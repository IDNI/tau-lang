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
#include "ba_provider.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>

namespace idni::tau_lang {

// ── Function mode ────────────────────────────────────────────────────────────

enum class FuncMode {
	DYNAMIC,  // graph chosen independently per bounded temporal window
	STATIC    // one global function fixed for the entire trace
};

// ── Function declaration ─────────────────────────────────────────────────────

struct FuncDecl {
	std::string name;                  // function symbol name
	FuncMode mode = FuncMode::DYNAMIC; // dynamic or static
	std::vector<std::string> arg_sorts; // argument sort names (for display)
	std::string result_sort;           // result sort name (for display)
	std::vector<size_t> arg_type_ids;  // resolved BA type IDs for arguments
	size_t result_type_id = 0;        // resolved BA type ID for result

	FuncDecl() = default;
	FuncDecl(std::string n, FuncMode m, std::vector<std::string> args,
		std::string res)
		: name(std::move(n)), mode(m), arg_sorts(std::move(args)),
		  result_sort(std::move(res)) {}
	FuncDecl(std::string n, FuncMode m, std::vector<std::string> args,
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

struct SupportBound {
	std::string func_name;
	std::vector<std::vector<std::string>> tuples; // each tuple is a vector of sort values
};

// ── Graph profile ────────────────────────────────────────────────────────────
// A purified graph edge: f(u1,...,uk) -> v

struct GraphEdge {
	std::string func_name;
	std::vector<std::string> args;   // argument variable/term names
	std::string result_var;          // result variable name
	bool is_static;                  // true if from static function
};

// A finite graph profile: a complete joint type + set of purified edges.

struct GraphProfile {
	size_t type_id;                  // complete joint type index
	std::vector<GraphEdge> edges;
};

// ── Term closure ─────────────────────────────────────────────────────────────
// The finite set of variables, stream values, constants, and function-result
// terms obtained by closing the atoms of a window under subterms.

struct TermClosure {
	std::vector<std::string> variables;
	std::vector<std::pair<std::string, std::vector<std::string>>> func_apps;
	// func_apps: (func_name, [arg1, arg2, ...])
	std::map<std::string, std::string> purified_vars;
	// maps "f(u1,...,uk)" -> "v_f_u1_..._uk"
};

// ── Static profile state ─────────────────────────────────────────────────────
// For each static f : S1 x ... x Sk -> T with support bound B_f,
// the global profile state is a finite partial map G_f : B_f -> R_f.

struct StaticProfileState {
	std::string func_name;
	std::map<std::string, std::string> bindings;
	// maps "arg_tuple_key" -> "result_type" (empty = unbound/bot)
};

// ── OCFuncs context ──────────────────────────────────────────────────────────
// The compilation context for the OCFuncs algorithm.

struct OCFuncsContext {
	std::vector<FuncDecl> declarations;
	std::map<std::string, SupportBound> support_bounds; // static funcs only
	std::map<std::string, StaticProfileState> static_states;
	std::vector<std::string> propositions;  // generated atomic propositions
};

// ── Step 1: Mode and support checking ────────────────────────────────────────

// Validate all function declarations and compute support bounds.
// Returns false if any function lacks a mode or a static function has no
// finite support certificate.
bool ocfuncs_check_modes(const std::vector<FuncDecl>& decls,
	OCFuncsContext& ctx);

// ── Step 2: Window purification ──────────────────────────────────────────────

// For each syntactic window shape, compute the finite term closure and
// purify all function terms. Returns purified edges separated by mode.
template <NodeType node>
TermClosure ocfuncs_purify_window(tref window_fm,
	const std::vector<FuncDecl>& decls);

// ── Step 3: Local legal profiles ─────────────────────────────────────────────

// For each purified context, enumerate legal graph profiles.
// Dynamic edges are checked within the window.
// Static edges are checked relative to the finite support profile.
template <NodeType node>
std::vector<GraphProfile> ocfuncs_enumerate_profiles(
	const TermClosure& closure,
	const OCFuncsContext& ctx);

// ── Step 4: Static profile alphabet ──────────────────────────────────────────

// Encode each static function's profile state propositionally.
// Returns the set of propositions and their bit-width encoding.
std::vector<std::string> ocfuncs_encode_static_profiles(
	const OCFuncsContext& ctx);

// ── Step 5: Quantifier elimination ───────────────────────────────────────────

// Eliminate quantifiers using parent/extension relations.
template <NodeType node>
tref ocfuncs_eliminate_quantifiers(tref fm, const OCFuncsContext& ctx);

// ── Step 6: Temporal LTL generation ──────────────────────────────────────────

// Generate the final propositional LTL formula.
template <NodeType node>
tref ocfuncs_generate_ltl(tref fm, const OCFuncsContext& ctx);

// ── Main entry point ─────────────────────────────────────────────────────────

// Compile a specification with heterogeneous function symbols into a
// propositional LTL formula suitable for synthesis.
//
// This is the OCFuncs algorithm: Steps 1-6.
template <NodeType node>
tref ocfuncs_compile(tref fm, const std::vector<FuncDecl>& decls);

// ── Function declaration extraction ──────────────────────────────────────────

// Extract FuncDecl from a parsed func_decl AST node.
template <NodeType node>
FuncDecl extract_func_decl(tref decl_node);

// Resolve sort name strings to BA type IDs using ba_types infrastructure.
// Populates arg_type_ids and result_type_id from arg_sorts and result_sort.
template <NodeType node>
void resolve_func_decl_types(FuncDecl& decl);

// Resolve all declarations in a vector.
template <NodeType node>
void resolve_func_decl_types(std::vector<FuncDecl>& decls);

// Check if a formula contains function applications
template <NodeType node>
bool has_func_applications(tref fm);

// ── Generic compile with provider ────────────────────────────────────────────

// Compile with an explicit BAProvider (for generic use across all ABAs).
// The provider is used for type enumeration, projection, satisfaction checks.
template <NodeType node, BAProvider Provider>
tref ocfuncs_compile(tref fm, const std::vector<FuncDecl>& decls,
	Provider& provider);

} // namespace idni::tau_lang

#include "ocfuncs.tmpl.h"

#endif // __IDNI__TAU__OCFUNCS_H__
