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

inline bool ocfuncs_check_modes(const std::vector<FuncDecl>& decls,
		OCFuncsContext& ctx) {
	ctx.declarations = decls;
	for (const auto& d : decls) {
		if (d.name.empty()) return false;
		if (d.arg_sorts.empty()) return false;
		if (d.result_sort.empty()) return false;

		if (d.mode == FuncMode::STATIC) {
			// Static functions require a finite support certificate.
			// For now, accept all declared static functions and require
			// the user to provide support bounds via constraints.
			if (ctx.support_bounds.find(d.name) ==
					ctx.support_bounds.end()) {
				// Initialize with empty support bound — will be
				// populated during analysis or by user declaration.
				ctx.support_bounds[d.name] = SupportBound{d.name, {}};
			}
			ctx.static_states[d.name] = StaticProfileState{d.name, {}};
		}
	}
	return true;
}

// ── Step 2: Window purification ──────────────────────────────────────────────

template <NodeType node>
TermClosure ocfuncs_purify_window(tref window_fm,
		const std::vector<FuncDecl>& decls) {
	using tau = tree<node>;
	TermClosure closure;

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
std::vector<GraphProfile> ocfuncs_enumerate_profiles(
		const TermClosure& closure,
		const OCFuncsContext& ctx) {
	std::vector<GraphProfile> profiles;

	// Build graph edges from purified function applications
	std::vector<GraphEdge> edges;
	for (const auto& [key, pvar] : closure.purified_vars) {
		GraphEdge edge;
		// Parse function name from key
		auto paren = key.find('(');
		if (paren == std::string::npos) continue;
		edge.func_name = key.substr(0, paren);
		edge.result_var = pvar;

		// Determine if static
		edge.is_static = false;
		for (const auto& d : ctx.declarations) {
			if (d.name == edge.func_name
					&& d.mode == FuncMode::STATIC) {
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
		GraphProfile p;
		p.type_id = 0;
		p.edges = edges;
		profiles.push_back(p);
	}

	return profiles;
}

// ── Step 4: Static profile alphabet ──────────────────────────────────────────

inline std::vector<std::string> ocfuncs_encode_static_profiles(
		const OCFuncsContext& ctx) {
	std::vector<std::string> props;

	for (const auto& [fname, state] : ctx.static_states) {
		// Find the declaration
		const FuncDecl* decl = nullptr;
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
tref ocfuncs_eliminate_quantifiers(tref fm, const OCFuncsContext& /* ctx */) {
	// For v1, quantifier elimination delegates to the existing QE pipeline.
	// The OCFuncs-specific extension is that parent/extension relations
	// must account for function-result variables.
	// Currently, pass through — the existing normalizer handles QE.
	return fm;
}

// ── Step 6: Temporal LTL generation ──────────────────────────────────────────

template <NodeType node>
tref ocfuncs_generate_ltl(tref fm, const OCFuncsContext& ctx) {
	// Replace function applications with their purified variables
	// and add congruence + static consistency constraints.

	// For each window, walk the formula and replace bf_func_app nodes
	// with the corresponding purified variable.
	struct FuncReplacer {
		const OCFuncsContext& ctx;
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

	FuncReplacer replacer{ctx, {}};
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
tref ocfuncs_compile(tref fm, const std::vector<FuncDecl>& decls) {
	OCFuncsContext ctx;

	// Step 1: Mode and support checking
	if (!ocfuncs_check_modes(decls, ctx)) {
		return nullptr; // reject: invalid declarations
	}

	// Step 2: Window purification
	TermClosure closure = ocfuncs_purify_window<node>(fm, decls);

	// Step 3: Local legal profiles
	auto profiles = ocfuncs_enumerate_profiles<node>(closure, ctx);

	// Step 4: Static profile alphabet
	auto static_props = ocfuncs_encode_static_profiles(ctx);
	ctx.propositions = static_props;

	// Step 5: Quantifier elimination
	tref qe_fm = ocfuncs_eliminate_quantifiers<node>(fm, ctx);

	// Step 6: Temporal LTL generation
	return ocfuncs_generate_ltl<node>(qe_fm, ctx);
}

// ── Function declaration extraction ──────────────────────────────────────────

template <NodeType node>
FuncDecl extract_func_decl(tref decl_node) {
	using tau = tree<node>;
	FuncDecl decl;

	const auto& t = tau::get(decl_node);
	if (!t.has_child()) return decl;

	// func_decl => func_mode func_sym ':' func_sort_list '->' type
	auto& root = t[0];

	// Extract mode
	for (size_t i = 0; i < root.children_size(); ++i) {
		auto& child = tau::get(root.child(i));
		if (child.is(tau::func_dynamic)) {
			decl.mode = FuncMode::DYNAMIC;
		} else if (child.is(tau::func_static)) {
			decl.mode = FuncMode::STATIC;
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
	// Type IDs start at 1; 0 = untyped.
	// Iterate registered types — the pool is small (< 64 types).
	// get_ba_type_name(id) returns ":untyped" for id=0 and falls back to
	// type 0's name for out-of-range IDs, so we stop when we see that.
	const std::string untyped = get_ba_type_name<node>(0);
	for (size_t i = 1; i < 64; ++i) {
		std::string name = get_ba_type_name<node>(i);
		if (name == untyped) break; // past end of registered types
		if (name == sort_name) return i;
	}
	return 0;
}

template <NodeType node>
void resolve_func_decl_types(FuncDecl& decl) {
	decl.arg_type_ids.clear();
	decl.arg_type_ids.reserve(decl.arg_sorts.size());
	for (const auto& sort_name : decl.arg_sorts)
		decl.arg_type_ids.push_back(resolve_sort_name_to_id<node>(sort_name));
	decl.result_type_id = resolve_sort_name_to_id<node>(decl.result_sort);
}

template <NodeType node>
void resolve_func_decl_types(std::vector<FuncDecl>& decls) {
	for (auto& d : decls)
		resolve_func_decl_types<node>(d);
}

// ── Generic compile with BAProvider ──────────────────────────────────────────

template <NodeType node, BAProvider Provider>
tref ocfuncs_compile(tref fm, const std::vector<FuncDecl>& decls,
		Provider& /* provider */) {
	// Currently delegates to the standard pipeline.
	// The provider will be used for type enumeration and satisfaction
	// as the individual steps are made provider-aware.
	return ocfuncs_compile<node>(fm, decls);
}

// ── Detection ────────────────────────────────────────────────────────────────

template <NodeType node>
bool has_func_applications(tref fm) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(fm).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		return t[0].value.nt == tau::bf_func_app;
	}) != nullptr;
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

} // namespace idni::tau_lang
