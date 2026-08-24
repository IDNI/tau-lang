// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// OCFuncs: Compiling Dynamic and Static Functions over Omega-Categorical Sorts
//
// Implements the OCFuncs algorithm for heterogeneous function symbols with
// arguments and results from different OMCAT sorts.  Functions are declared
// either "dynamic" (per-window independence) or "static" (one global graph
// for the entire trace).
//
// Reference: OCFuncs design paper (naturalog/agents/docs/research/ocfuncs.tex)

#ifndef __IDNI__TAU__OCFUNCS_H__
#define __IDNI__TAU__OCFUNCS_H__

#include "normalizer.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>

namespace idni::tau_lang {

// ── Function mode ────────────────────────────────────────────────────────────

enum class func_mode {
	DYNAMIC,  // graph chosen independently per bounded temporal window
	STATIC    // one global function fixed for the entire trace
};

// ── Function declaration ─────────────────────────────────────────────────────

struct func_decl {
	std::string name;                  // function symbol name
	func_mode mode = func_mode::DYNAMIC; // dynamic or static
	std::vector<std::string> arg_sorts; // argument sort names (for display)
	std::string result_sort;           // result sort name (for display)
	std::vector<size_t> arg_type_ids;  // resolved BA type IDs for arguments
	size_t result_type_id = 0;        // resolved BA type ID for result

	func_decl() = default;
	func_decl(std::string n, func_mode m, std::vector<std::string> args,
		std::string res)
		: name(std::move(n)), mode(m), arg_sorts(std::move(args)),
		  result_sort(std::move(res)) {}
	func_decl(std::string n, func_mode m, std::vector<std::string> args,
		std::string res, std::vector<size_t> atids, size_t rtid)
		: name(std::move(n)), mode(m), arg_sorts(std::move(args)),
		  result_sort(std::move(res)), arg_type_ids(std::move(atids)),
		  result_type_id(rtid) {}

	size_t arity() const { return arg_sorts.size(); }

	bool types_resolved() const {
		if (result_type_id == 0) return false;
		for (size_t id : arg_type_ids) if (id == 0) return false;
		return true;
	}
};

// ── Static support bound ─────────────────────────────────────────────────────
// For a static function f, B_f is the finite set of argument tuples on which
// the specification can ever inspect f.

struct support_bound {
	std::string func_name;
	std::vector<std::vector<std::string>> tuples; // each tuple is a vector of sort values
};

// ── Graph profile ────────────────────────────────────────────────────────────
// A purified graph edge: f(u1,...,uk) -> v

struct graph_edge {
	std::string func_name;
	std::vector<std::string> args;   // argument variable/term names
	std::string result_var;          // result variable name
	bool is_static;                  // true if from static function
};

// A finite graph profile: a complete joint type + set of purified edges.

struct graph_profile {
	size_t type_id;                  // complete joint type index
	std::vector<graph_edge> edges;
};

// ── Term closure ─────────────────────────────────────────────────────────────
// The finite set of variables, stream values, constants, and function-result
// terms obtained by closing the atoms of a window under subterms.

struct term_closure {
	std::vector<std::string> variables;
	std::vector<std::pair<std::string, std::vector<std::string>>> func_apps;
	// func_apps: (func_name, [arg1, arg2, ...])
	std::map<std::string, std::string> purified_vars;
	// maps "f(u1,...,uk)" -> "v_f_u1_..._uk"
};

// ── Static profile state ─────────────────────────────────────────────────────
// For each static f : S1 x ... x Sk -> T with support bound B_f,
// the global profile state is a finite partial map G_f : B_f -> R_f.

struct static_profile_state {
	std::string func_name;
	std::map<std::string, std::string> bindings;
	// maps "arg_tuple_key" -> "result_type" (empty = unbound/bot)
};

// ── OCFuncs context ──────────────────────────────────────────────────────────
// The compilation context for the OCFuncs algorithm.

struct ocfuncs_context {
	std::vector<func_decl> declarations;
	std::map<std::string, support_bound> support_bounds; // static funcs only
	std::map<std::string, static_profile_state> static_states;
	std::vector<std::string> propositions;  // generated atomic propositions
};

// ── Step 1: Mode and support checking ────────────────────────────────────────

// Validate all function declarations and compute support bounds.
// Returns false if any function lacks a mode or a static function has no
// finite support certificate.
bool ocfuncs_check_modes(const std::vector<func_decl>& decls,
	ocfuncs_context& ctx);

// ── Step 2: Window purification ──────────────────────────────────────────────

// For each syntactic window shape, compute the finite term closure and
// purify all function terms. Returns purified edges separated by mode.
template <NodeType node>
term_closure ocfuncs_purify_window(tref window_fm,
	const std::vector<func_decl>& decls);

// ── Step 3: Local legal profiles ─────────────────────────────────────────────

// For each purified context, enumerate legal graph profiles.
// Dynamic edges are checked within the window.
// Static edges are checked relative to the finite support profile.
template <NodeType node>
std::vector<graph_profile> ocfuncs_enumerate_profiles(
	const term_closure& closure,
	const ocfuncs_context& ctx);

// ── Step 4: Static profile alphabet ──────────────────────────────────────────

// Encode each static function's profile state propositionally.
// Returns the set of propositions and their bit-width encoding.
std::vector<std::string> ocfuncs_encode_static_profiles(
	const ocfuncs_context& ctx);

// ── Step 5: Quantifier elimination ───────────────────────────────────────────

// Eliminate quantifiers using parent/extension relations.
template <NodeType node>
tref ocfuncs_eliminate_quantifiers(tref fm, const ocfuncs_context& ctx);

// ── Step 6: Temporal LTL generation ──────────────────────────────────────────

// Generate the final propositional LTL formula.
template <NodeType node>
tref ocfuncs_generate_ltl(tref fm, const ocfuncs_context& ctx);

// ── Main entry point ─────────────────────────────────────────────────────────

// Compile a specification with heterogeneous function symbols into a
// propositional LTL formula suitable for synthesis.
//
// This is the OCFuncs algorithm: Steps 1-6.
template <NodeType node>
tref ocfuncs_compile(tref fm, const std::vector<func_decl>& decls);

// ── Function declaration extraction ──────────────────────────────────────────

// Extract func_decl from a parsed func_decl AST node.
template <NodeType node>
func_decl extract_func_decl(tref decl_node);

// Resolve sort name strings to BA type IDs using ba_types infrastructure.
// Populates arg_type_ids and result_type_id from arg_sorts and result_sort.
template <NodeType node>
void resolve_func_decl_types(func_decl& decl);

// Resolve all declarations in a vector.
template <NodeType node>
void resolve_func_decl_types(std::vector<func_decl>& decls);

// Check if a formula contains function applications
template <NodeType node>
bool has_func_applications(tref fm);

} // namespace idni::tau_lang

#include "ocfuncs.tmpl.h"

#endif // __IDNI__TAU__OCFUNCS_H__
