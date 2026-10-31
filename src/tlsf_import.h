// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// `tau import-tlsf`: a TLSF basic specification (the SYNTCOMP format, no
// parameters) as a Tau specification over bv[1] streams.
//
// Each input and output of the TLSF file becomes a bv[1] console stream of
// the same name, a proposition `a` the atom `a[t]:bv[1] = 1` under a temporal
// operator and `a[0]:bv[1] = 1` outside one, since a TLSF formula is read at
// step 0, and `X φ` the formula φ read one step later (`a[t+1]` or `a[1]`).
// The sections combine as TLSF's Mealy semantics says:
//
//   INITIALLY -> (PRESET && ((G REQUIRE) -> (ASSUME -> ((G ASSERT) && GUARANTEE))))
//
// an empty section being true and left out, so a specification of
// guarantees only is a conjunction of clauses. Strict and Moore semantics
// are refused.

#ifndef __IDNI__TAU__TLSF_IMPORT_H__
#define __IDNI__TAU__TLSF_IMPORT_H__

#include <set>
#include <string>
#include <vector>

#include "tlsf_parser.generated.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

namespace tlsf_detail {

using ptree = tlsf_parser::tree;
using nt = tlsf_parser_nonterminals;

inline size_t nt_of(tref n) { return ptree::get(n).get_nt(); }

inline trefs children(tref n) { return ptree::get(n).get_children(); }

// The children of n that are nonterminals of kind k, at any depth below
// nodes of other kinds.
inline void collect(tref n, size_t k, trefs& out) {
	for (tref c : children(n)) {
		if (ptree::get(c).is_t()) continue;
		if (nt_of(c) == k) out.push_back(c);
		else collect(c, k, out);
	}
}

inline std::string ident_of(tref n) {
	trefs names;
	if (nt_of(n) == nt::name) names.push_back(n);
	else collect(n, nt::name, names);
	return names.empty() ? std::string()
		: ptree::get(names[0]).get_terminals();
}

struct converter {
	std::set<std::string> inputs, outputs;
	// The text of n read `ahead` steps later, under a temporal operator
	// when `temporal`.
	result<std::string> fm(tref n, int_t ahead, bool temporal) {
		result<std::string> r;
		auto sub = [&](size_t i, int_t a, bool tm) -> result<std::string> {
			trefs fs;
			for (tref c : children(n)) if (!ptree::get(c).is_t())
				fs.push_back(c);
			if (i >= fs.size()) {
				result<std::string> e;
				return e.with_error(code::parse_error,
					"a TLSF formula is incomplete");
			}
			return fm(fs[i], a, tm);
		};
		auto bin = [&](const char* op, bool tm) -> result<std::string> {
			result<std::string> r;
			TAU_TRY(auto a, sub(0, ahead, tm));
			TAU_TRY(auto b, sub(1, ahead, tm));
			return r.with_value("(" + a + " " + op + " " + b + ")");
		};
		auto un = [&](const char* op, bool tm) -> result<std::string> {
			result<std::string> r;
			TAU_TRY(auto a, sub(0, ahead, tm));
			return r.with_value("(" + std::string(op) + " " + a + ")");
		};
		switch (nt_of(n)) {
		case nt::fm: case nt::imp: case nt::disj: case nt::conj:
		case nt::temp: case nt::unary: return sub(0, ahead, temporal);
		case nt::f_equiv: return bin("<->", temporal);
		case nt::f_imply: return bin("->", temporal);
		case nt::f_or: return bin("||", temporal);
		case nt::f_and: return bin("&&", temporal);
		case nt::f_until: return bin("U", true);
		case nt::f_release: return bin("R", true);
		case nt::f_weak_until: return bin("W", true);
		case nt::f_not: return un("!", temporal);
		case nt::f_always: return un("always", true);
		case nt::f_sometimes: return un("sometimes", true);
		case nt::f_next: return sub(0, ahead + 1, temporal);
		case nt::f_true: return r.with_value("T");
		case nt::f_false: return r.with_value("F");
		case nt::ident: {
			const std::string a = ident_of(n);
			if (!inputs.contains(a) && !outputs.contains(a))
				return r.with_error(code::parse_error,
					"the TLSF formula names a signal that is neither "
					"an input nor an output", {{label::name, a}});
			const std::string at = !temporal ? std::to_string(ahead)
				: ahead ? "t+" + std::to_string(ahead) : "t";
			return r.with_value("(" + a + "[" + at + "]:bv[1] = 1)");
		}
		default: break;
		}
		return sub(0, ahead, temporal);
	}
};

// The names Tau reads as operators or constants, which a stream cannot take.
inline bool reserved(const std::string& s) {
	static const std::set<std::string> words{ "T", "F", "G", "U", "R", "W",
		"S", "O", "H", "A", "E", "always", "sometimes", "once",
		"historically", "until", "since", "trigger", "release", "ex", "all",
		"this", "u" };
	return words.contains(s);
}

} // namespace tlsf_detail

/**
 * @brief The Tau specification of the TLSF basic specification @p text.
 * @return The specification text, with a stream definition per signal; an
 * error for text that is not TLSF basic, for strict or Moore semantics, and
 * for a signal whose name Tau reads as an operator.
 */
inline result<std::string> tlsf_to_tau(const std::string& text) {
	using namespace tlsf_detail;
	result<std::string> r;
	auto parsed = tlsf_parser::instance().parse(text.c_str(), text.size());
	if (!parsed.found) return r.with_error(code::parse_error,
		"the file is not a TLSF basic specification",
		{{label::value, truncate_for_message(parsed.parse_error.to_str(
			tlsf_parser::error::info_lvl::INFO_BASIC))}});
	tref root = parsed.get_shaped_tree2();
	trefs sems;
	collect(root, nt::semantics, sems);
	for (tref s : sems) {
		trefs ids;
		collect(s, nt::ident, ids);
		for (tref i : ids) if (ident_of(i) != "Mealy")
			return r.with_error(code::unsupported_operation,
				"tau imports TLSF with Mealy semantics only",
				{{label::value, ident_of(i)}});
	}
	// SEMANTICS and TARGET are independent: a Mealy specification can ask
	// for a Moore implementation, whose output cannot see the current input
	trefs targets;
	collect(root, nt::target, targets);
	for (tref t : targets) {
		trefs ids;
		collect(t, nt::ident, ids);
		for (tref i : ids) if (ident_of(i) != "Mealy")
			return r.with_error(code::unsupported_operation,
				"tau imports TLSF with a Mealy target only",
				{{label::value, ident_of(i)}});
	}
	converter cv;
	auto signals = [&](size_t kind, std::set<std::string>& into)
		-> result<void>
	{
		result<void> rv;
		trefs secs;
		collect(root, kind, secs);
		for (tref sec : secs) {
			trefs ids;
			collect(sec, nt::ident, ids);
			for (tref i : ids) {
				const std::string a = ident_of(i);
				if (reserved(a)) return rv.with_error(
					code::unsupported_operation,
					"a TLSF signal is named like a Tau operator",
					{{label::name, a}});
				into.insert(a);
			}
		}
		return rv;
	};
	TAU_TRY_VOID(signals(nt::inputs, cv.inputs));
	TAU_TRY_VOID(signals(nt::outputs, cv.outputs));
	// the formulas of the sections of `kind`, read under `always` when
	// `temporal`
	auto section = [&](size_t kind, bool temporal)
		-> result<std::vector<std::string>>
	{
		result<std::vector<std::string>> r;
		trefs secs, fms;
		collect(root, kind, secs);
		for (tref sec : secs) collect(sec, nt::fm, fms);
		std::vector<std::string> out;
		for (tref f : fms) {
			TAU_TRY(auto s, cv.fm(f, 0, temporal));
			out.push_back(s);
		}
		return r.with_value(std::move(out));
	};
	auto conj = [](const std::vector<std::string>& fs) {
		std::string out;
		for (const auto& f : fs) out += (out.empty() ? "" : " && ") + f;
		return fs.size() == 1 ? out : "(" + out + ")";
	};
	TAU_TRY(auto initially, section(nt::initially, false));
	TAU_TRY(auto preset, section(nt::preset, false));
	TAU_TRY(auto require, section(nt::require, true));
	TAU_TRY(auto assume, section(nt::assume, false));
	TAU_TRY(auto assert_, section(nt::assert, true));
	TAU_TRY(auto guarantee, section(nt::guarantee, false));
	std::vector<std::string> goal;
	if (!assert_.empty()) goal.push_back("(always " + conj(assert_) + ")");
	goal.insert(goal.end(), guarantee.begin(), guarantee.end());
	if (goal.empty()) goal.push_back("T");
	if (!assume.empty()) goal = { "(" + conj(assume) + " -> " + conj(goal) + ")" };
	if (!require.empty())
		goal = { "((always " + conj(require) + ") -> " + conj(goal) + ")" };
	goal.insert(goal.begin(), preset.begin(), preset.end());
	std::string main;
	for (const auto& g : goal) main += (main.empty() ? "" : " && ") + g;
	if (!initially.empty())
		main = conj(initially) + " -> (" + main + ")";
	std::string spec;
	for (const auto& a : cv.inputs) spec += a + " : bv[1] := in console.\n";
	for (const auto& a : cv.outputs) spec += a + " : bv[1] := out console.\n";
	spec += main + "\n";
	return r.with_value(spec);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TLSF_IMPORT_H__
