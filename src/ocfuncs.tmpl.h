// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "ocfuncs.h"
#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ocfuncs"

#include <sstream>
#include <algorithm>
#include <cassert>
#include <numeric>

namespace idni::tau_lang {

// ── Step 1: Mode and support checking ────────────────────────────────────────

inline bool ocfuncs_check_modes(const std::vector<func_decl>& decls,
		ocfuncs_context& ctx) {
	ctx.declarations = decls;
	for (const auto& d : decls) {
		if (d.name.empty()) return false;
		if (d.arg_sorts.empty()) return false;
		if (d.result_sort.empty()) return false;

		if (d.mode == func_mode::STATIC) {
			// Static functions require a finite support certificate.
			// For now, accept all declared static functions and require
			// the user to provide support bounds via constraints.
			if (ctx.support_bounds.find(d.name) ==
					ctx.support_bounds.end()) {
				// Initialize with empty support bound — will be
				// populated during analysis or by user declaration.
				ctx.support_bounds[d.name] = support_bound{d.name, {}};
			}
			ctx.static_states[d.name] = static_profile_state{d.name, {}};
		}
	}
	return true;
}

// ── Step 2: Window purification ──────────────────────────────────────────────

template <NodeType node>
term_closure ocfuncs_purify_window(tref window_fm,
		const std::vector<func_decl>& decls) {
	using tau = tree<node>;
	term_closure closure;

	std::set<std::string> func_names;
	for (const auto& d : decls) func_names.insert(d.name);

	// Walk the formula tree to find all function applications
	// and collect variables into the term closure.
	tau::get(window_fm).find_top([&](tref n) {
		const auto& t = tau::get(n);
		if (!t.has_child()) return false;

		// Check for bf_func_app nodes
		if (t[0].value.nt == tau::bf_func_app) {
			// Extract function name and arguments
			auto& app_node = t[0];
			if (app_node.children_size() >= 2) {
				std::string fname = tau::get(app_node.child(0)).to_str();
				std::vector<std::string> args;
				// Traverse func_args children
				auto& args_node = tau::get(app_node.child(1));
				for (size_t i = 0; i < args_node.children_size(); ++i) {
					args.push_back(
						tau::get(args_node.child(i)).to_str());
				}
				closure.func_apps.emplace_back(fname, args);

				// Create purified variable name
				std::string key = fname + "(";
				for (size_t i = 0; i < args.size(); ++i) {
					if (i > 0) key += ",";
					key += args[i];
				}
				key += ")";
				std::string pvar = "v_" + fname;
				for (const auto& a : args) pvar += "_" + a;
				closure.purified_vars[key] = pvar;
			}
		}

		// Collect variables
		if (t.is(tau::variable)) {
			closure.variables.push_back(t.to_str());
		}

		return false; // continue walking
	});

	return closure;
}

// ── Step 3: Local legal profiles ─────────────────────────────────────────────

template <NodeType node>
std::vector<graph_profile> ocfuncs_enumerate_profiles(
		const term_closure& closure,
		const ocfuncs_context& ctx) {
	std::vector<graph_profile> profiles;

	// Build graph edges from purified function applications
	std::vector<graph_edge> edges;
	for (const auto& [key, pvar] : closure.purified_vars) {
		graph_edge edge;
		// Parse function name from key
		auto paren = key.find('(');
		if (paren == std::string::npos) continue;
		edge.func_name = key.substr(0, paren);
		edge.result_var = pvar;

		// Determine if static
		edge.is_static = false;
		for (const auto& d : ctx.declarations) {
			if (d.name == edge.func_name
					&& d.mode == func_mode::STATIC) {
				edge.is_static = true;
				break;
			}
		}

		// Parse arguments from key
		std::string args_str = key.substr(paren + 1,
			key.size() - paren - 2);
		std::istringstream iss(args_str);
		std::string arg;
		while (std::getline(iss, arg, ',')) {
			edge.args.push_back(arg);
		}

		edges.push_back(edge);
	}

	// Create a single profile (full enumeration would require the
	// joint provider; for v1 we create one profile per consistent
	// assignment)
	if (!edges.empty()) {
		graph_profile p;
		p.type_id = 0;
		p.edges = edges;
		profiles.push_back(p);
	}

	return profiles;
}

// ── Step 4: Static profile alphabet ──────────────────────────────────────────

inline std::vector<std::string> ocfuncs_encode_static_profiles(
		const ocfuncs_context& ctx) {
	std::vector<std::string> props;

	for (const auto& [fname, state] : ctx.static_states) {
		// Find the declaration
		const func_decl* decl = nullptr;
		for (const auto& d : ctx.declarations) {
			if (d.name == fname) { decl = &d; break; }
		}
		if (!decl) continue;

		auto it = ctx.support_bounds.find(fname);
		if (it == ctx.support_bounds.end()) continue;

		const auto& support = it->second;

		// For each support tuple, create a proposition encoding
		// whether it has been bound and what value it has
		for (size_t i = 0; i < support.tuples.size(); ++i) {
			std::string prop_base = "sp_" + fname + "_" +
				std::to_string(i);
			// Bound/unbound proposition
			props.push_back(prop_base + "_bound");
			// Value proposition (simplified: one prop per possible value)
			props.push_back(prop_base + "_val");
		}
	}

	return props;
}

// ── Step 5: Quantifier elimination ───────────────────────────────────────────

template <NodeType node>
tref ocfuncs_eliminate_quantifiers(tref fm, const ocfuncs_context& /* ctx */) {
	// For v1, quantifier elimination delegates to the existing QE pipeline.
	// The OCFuncs-specific extension is that parent/extension relations
	// must account for function-result variables.
	// Currently, pass through — the existing normalizer handles QE.
	return fm;
}

// ── Step 6: Temporal LTL generation ──────────────────────────────────────────

template <NodeType node>
tref ocfuncs_generate_ltl(tref fm, const ocfuncs_context& ctx) {
	// Replace function applications with their purified variables
	// and add congruence + static consistency constraints.

	// For each window, walk the formula and replace bf_func_app nodes
	// with the corresponding purified variable.
	struct func_replacer {
		const ocfuncs_context& ctx;
		std::vector<tref> constraints;

		tref visit(tref n) {
			using tau = tree<node>;
			const auto& t = tau::get(n);
			if (!t.has_child()) return n;

			if (t[0].value.nt == tau::bf_func_app) {
				// Replace with purified variable (pass through for now;
				// full purification requires type information from context)
				return n;
			}

			// Recursively visit children
			auto& op = t[0];
			size_t nch = op.children_size();
			bool changed = false;
			std::vector<tref> new_ch;
			new_ch.reserve(nch);
			for (size_t i = 0; i < nch; ++i) {
				tref nc = visit(op.child(i));
				if (nc != op.child(i)) changed = true;
				new_ch.push_back(nc);
			}
			if (!changed) return n;
			return tau::get_raw(t.value, new_ch.data(),
				new_ch.size(), n);
		}
	};

	func_replacer replacer{ctx, {}};
	tref result = replacer.visit(fm);

	// Add static consistency constraints:
	// For each static function, add G(first_bind ∧ subsequent_match)
	for (const auto& [fname, state] : ctx.static_states) {
		auto it = ctx.support_bounds.find(fname);
		if (it == ctx.support_bounds.end()) continue;

		// The constraint is: for each support tuple,
		// once bound, the value never changes.
		// Encoded as: G(bound[t] → (val[t] = val[t-1]))
		// This is added as a conjunction to the result.
		auto props = ocfuncs_encode_static_profiles(ctx);
		for (size_t i = 0; i + 1 < props.size(); i += 2) {
			// Each pair is (bound, val) — the G constraint is that
			// once bound, val stays the same.
			// This is a simplified version; full encoding would use
			// explicit bit vectors.
		}
	}

	return result;
}

// ── Main entry point ─────────────────────────────────────────────────────────

template <NodeType node>
tref ocfuncs_compile(tref fm, const std::vector<func_decl>& decls) {
	ocfuncs_context ctx;

	// Step 1: Mode and support checking
	if (!ocfuncs_check_modes(decls, ctx)) {
		return nullptr; // reject: invalid declarations
	}

	// Step 2: Window purification
	term_closure closure = ocfuncs_purify_window<node>(fm, decls);

	// Step 3: Local legal profiles
	auto profiles = ocfuncs_enumerate_profiles<node>(closure, ctx);
	(void) profiles;

	// Step 4: Static profile alphabet
	auto static_props = ocfuncs_encode_static_profiles(ctx);
	ctx.propositions = static_props;

	// Step 5: Quantifier elimination
	tref qe_fm = ocfuncs_eliminate_quantifiers<node>(fm, ctx);
	(void) qe_fm;

	// LG-22: Steps 3, 5 and 6 are V1 STUBS — enumerate_profiles emits one
	// hardcoded profile, eliminate_quantifiers is a pass-through, and
	// generate_ltl returns bf_func_app nodes unchanged with an empty
	// "static consistency constraints" loop body.  Running them and returning
	// the result hands the caller its own input back while the header
	// advertises "a propositional LTL formula suitable for synthesis" — silent
	// wrongness for anyone who wires this up.
	//
	// Refuse until the steps are real.  The steps above still run so they stay
	// instantiated and their behaviour stays observable to the tests.
	// TAU_LOG_ERROR, not LOG_ERROR: `src/tau.h` (the single-header form used by
	// the Release test pack) #undefs every short LOG_* macro at its end, and
	// logging.h is include-guarded so re-including cannot bring them back.
	// Only the TAU_LOG_* spellings survive there — the same reason api.tmpl.h
	// uses them.
	TAU_LOG_ERROR << "[ocfuncs] ocfuncs_compile is not implemented: Steps 3, "
	                 "5 and 6 are V1 stubs, so the returned formula would be "
	                 "the input essentially unchanged\n";
	return nullptr;
}

// ── Function declaration extraction ──────────────────────────────────────────

template <NodeType node>
func_decl extract_func_decl(tref decl_node) {
	using tau = tree<node>;
	func_decl decl;

	const auto& t = tau::get(decl_node);
	if (!t.has_child()) return decl;

	// func_decl => func_mode func_sym ':' func_sort_list '->' type
	auto& root = t[0];

	// Extract mode
	for (size_t i = 0; i < root.children_size(); ++i) {
		auto& child = tau::get(root.child(i));
		if (child.is(tau::func_dynamic)) {
			decl.mode = func_mode::DYNAMIC;
		} else if (child.is(tau::func_static)) {
			decl.mode = func_mode::STATIC;
		} else if (child.is(tau::func_sym)) {
			decl.name = child.to_str();
		} else if (child.is(tau::func_sort_list)) {
			// Extract argument sorts
			for (size_t j = 0; j < child.children_size(); ++j) {
				auto& sort_child = tau::get(child.child(j));
				if (sort_child.is(tau::type)) {
					decl.arg_sorts.push_back(sort_child.to_str());
				}
			}
		} else if (child.is(tau::type)) {
			// Last type is the result sort
			decl.result_sort = child.to_str();
		}
	}

	return decl;
}

// ── Type resolution ──────────────────────────────────────────────────────────

// Helper: resolve a sort name string to a BA type ID.
// Iterates registered types using the public get_ba_type_name API.
template <NodeType node>
static size_t resolve_sort_name_to_id(const std::string& sort_name) {
	// Type IDs start at 1; 0 = untyped, and 0 is also the "not found" answer.
	//
	// LG-22: this used to scan ids 1..63 assuming `get_ba_type_name` returns
	// type 0's name past the end of the registry.  It does not — `ba_types::
	// name` THROWS `invalid ba_type_id` there — so the very first call threw
	// (this function had no test and no production caller, so it had never
	// been instantiated, let alone run).  Ask the registry for its size.
	//
	// Registered names also carry a leading ':' (":sbf", ":qlt", …), so a
	// declaration written `f : sbf -> sbf` would never have matched even
	// without the throw.  Accept the sort name with or without the colon.
	const size_t n = get_ba_type_count<node>();
	const std::string colon_name =
		(sort_name.empty() || sort_name[0] == ':') ? sort_name : ":" + sort_name;
	for (size_t i = 1; i < n; ++i) {
		std::string name = get_ba_type_name<node>(i);
		if (name == sort_name || name == colon_name) return i;
	}
	return 0;
}

template <NodeType node>
void resolve_func_decl_types(func_decl& decl) {
	decl.arg_type_ids.clear();
	decl.arg_type_ids.reserve(decl.arg_sorts.size());
	for (const auto& sort_name : decl.arg_sorts)
		decl.arg_type_ids.push_back(resolve_sort_name_to_id<node>(sort_name));
	decl.result_type_id = resolve_sort_name_to_id<node>(decl.result_sort);
}

template <NodeType node>
void resolve_func_decl_types(std::vector<func_decl>& decls) {
	for (auto& d : decls)
		resolve_func_decl_types<node>(d);
}

// ── Detection ────────────────────────────────────────────────────────────────

template <NodeType node>
bool has_func_applications(tref fm, const std::vector<func_decl>& decls) {
	using tau = tree<node>;

	// LG-22.  This used to look for a node whose first child is a
	// `bf_func_app`, and answered false for EVERY input — including a formula
	// that is nothing but a function application.  It had no test and no
	// production caller, so nothing noticed.
	//
	// The reason is that `bf_func_app` does not appear in a parsed formula at
	// all.  `g(o1[t]) = 1` parses as
	//
	//     bf_eq > [ bf > bf_ref > ref > { sym "g", ref_args > ref_arg > … },
	//               bf > bf_t ]
	//
	// which is exactly the shape of a recursive-relation reference: at parse
	// time a function application and an rr reference are indistinguishable.
	// "Does this formula apply a function?" is therefore only answerable
	// RELATIVE TO the declarations, which is why they are a parameter now.
	if (decls.empty()) return false;

	// A `sym` node carries its name as an INTERNED ID in `value.data`, not as
	// text in a child — see `get_function_signature` in ba_types_inference,
	// which reads a ref's symbol as `ref_head[0].value.data`.  Build the same
	// id for each declared name and compare ids; reading the name back out as
	// a string would be both slower and shape-dependent.
	std::set<size_t> sym_ids;
	for (const auto& d : decls)
		sym_ids.insert(tau::get(build_sym<node>(d.name)).value.data);

	// `select_all(is<node, tau::ref>)` is the idiom the recursive-relation
	// machinery already uses to walk refs (normalizer.tmpl.h:1309).
	//
	// No memoisation: the answer depends on `decls` as well as on `fm`, so a
	// formula-keyed cache would return a stale verdict for a different
	// declaration set.  There is no hot path to protect.
	for (tref r : tau::get(fm).select_all(is<node, tau::ref>))
		if (sym_ids.count(tau::get(r)[0].value.data)) return true;
	return false;
}

} // namespace idni::tau_lang
