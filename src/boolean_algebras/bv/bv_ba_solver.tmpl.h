// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv/bv_ba.h" // Only for IDE resolution, not really needed.
#include "boolean_algebras/bv/parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_solver"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

// Forward declaration: the 3-arg convenience overload below calls this
// (textually later in the file) with an explicit template argument, which
// needs the memo-taking overload already declared for ordinary unqualified
// lookup to find it.
template <NodeType node>
std::optional<bv> bv_eval_node(const typename tree<node>::traverser& form, subtree_map<node, bv>& vars,
			       subtree_map<node, bv>& free_vars, bv_eval_memo<node>& memo,
			       size_t& ctx_counter, size_t ctx);

template <NodeType node>
std::optional<bv> bv_eval_node(const typename tree<node>::traverser& form, subtree_map<node, bv>& vars,
			       subtree_map<node, bv>& free_vars) {
	// Fresh memo per top-level call: shared across the whole recursion but
	// must not survive past it (see the memo-taking overload below). ctx 0
	// is the "no binder active" context.
	bv_eval_memo<node> memo;
	size_t ctx_counter = 0;
	return bv_eval_node<node>(form, vars, free_vars, memo, ctx_counter, 0);
}

// Memoised worker. Caches tref -> cvc5::Term, keyed by (tref, ctx).
//
// A subtree's translation isn't context-free: `variable` resolves through
// `vars`, mutated (save/restore) by wff_all/wff_ex, which can shadow the
// same tref -- so a plain per-tref cache would replay a translation under
// the wrong binding.
//
// `ctx` identifies the enclosing binder instance instead (0 at top level,
// else a fresh ctx_counter value per binder entry). Each wff_all/wff_ex
// node is entered at most once per call, so every subtree reached while
// that instance is live sees the same `vars` -- memoizing per (tref, ctx)
// is sound even under shadowing.
template <NodeType node>
std::optional<bv> bv_eval_node(const typename tree<node>::traverser& form, subtree_map<node, bv>& vars,
			       subtree_map<node, bv>& free_vars, bv_eval_memo<node>& memo,
			       size_t& ctx_counter, size_t ctx) {
	using tau = tree<node>;
	using tt = typename tree<node>::traverser;

	// Walked with the library's pre_order visit, which is iterative: a
	// formula nests as deep as it likes, and a worker thread gets 512 KiB
	// on Darwin against 8 MiB on glibc, so recursing here is a crash.
	// `down` opens a node, `up` folds it once all its children are done.
	// There is no fold traversal in the tree API, so the children's
	// values are carried on an explicit value stack.

	// How a node consumes its children. Anything with no case below is a
	// leaf and evaluates to nullopt.
	enum kind { leaf, wrap, unary, binary, binder };
	auto kind_of = [](size_t nt) {
		switch (nt) {
		// wrappers: the value is the value of the single child
		case tau::wff_always: case tau::wff_sometimes:
		case tau::wff: case tau::bf: case tau::ctnvar:
			return wrap;
		case tau::wff_all: case tau::wff_ex:	return binder;
		case tau::wff_neg: case tau::bf_neg: case tau::bf_cast:
			return unary;
		case tau::wff_and: case tau::wff_or:
		case tau::bf_eq: case tau::bf_neq:
		case tau::bf_lteq: case tau::bf_nlteq:
		case tau::bf_gt: case tau::bf_ngt:
		case tau::bf_gteq: case tau::bf_ngteq:
		case tau::bf_lt: case tau::bf_nlt:
		case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
		case tau::bf_div: case tau::bf_mod:
		case tau::bf_and: case tau::bf_nand:
		case tau::bf_or: case tau::bf_nor:
		case tau::bf_xor: case tau::bf_xnor:
		case tau::bf_shl: case tau::bf_shr:
		case tau::bf_min: case tau::bf_max:	return binary;
		default:				return leaf;
		}
	};

	auto eval_leaf = [&](tref n, size_t nt) -> std::optional<bv> {
		switch (nt) {
		case tau::wff_t: return make_bitvector_true();
		case tau::wff_f: return make_bitvector_false();
		case tau::variable: {
			if (auto it = vars.find(n); it != vars.end())
				return it->second;
			if (auto it = free_vars.find(n); it != free_vars.end())
				return it->second;
			auto vn = (tt(n) | tt::Tree).to_str();
			// a new constant of the right type, remembered as free
			size_t bv_size = get_bv_size<node>(
				tau::get(n).get_ba_type_tree());
			// no builder wrapper for mkConst yet, unlike mkVar
			auto x = cvc5_term_manager.mkConst(
				cvc5_term_manager.mkBitVectorSort(bv_size),
				vn.c_str());
			free_vars.emplace(n, x);
			return std::optional<bv>(x);
		}
		case tau::ba_constant: {
			// is_bv_solvable_formula only inspects variable nodes, so a
			// formula whose variables are all bitvectors but which also
			// contains a non-bv (e.g. sbf or tau) constant passes that gate;
			// std::get would then throw std::bad_variant_access in release
			// (the DBG-only assert doesn't guard it there). Fail gracefully
			// instead, same as any other untranslatable node.
			auto cte = tt(n) | tt::ba_constant;
			if (!std::holds_alternative<bv>(cte)) return std::nullopt;
			return std::get<bv>(cte);
		}
		case tau::bf_t: {
			// Same rationale as the ba_constant case above: a non-bv
			// (e.g. sbf or tau) T/F node can reach here once every
			// variable has been substituted away, since
			// is_bv_solvable_formula only inspects variable nodes.
			// get_bv_size requires an explicit bitwidth; fail
			// gracefully instead of asserting/crashing.
			tref type_tree = tau::get(n).get_ba_type_tree();
			if (!is_bv_type_family<node>(tau::get(n).get_ba_type())
				|| !(tt(type_tree) | tau::type | tau::subtype))
				return std::nullopt;
			return make_bitvector_top_elem(get_bv_size<node>(type_tree));
		}
		case tau::bf_f: {
			tref type_tree = tau::get(n).get_ba_type_tree();
			if (!is_bv_type_family<node>(tau::get(n).get_ba_type())
				|| !(tt(type_tree) | tau::type | tau::subtype))
				return std::nullopt;
			return make_bitvector_bottom_elem(get_bv_size<node>(type_tree));
		}
		default: return std::nullopt;
		}
	};

	auto combine1 = [&](tref n, size_t nt, const std::optional<bv>& o)
		-> std::optional<bv>
	{
		if (!o) return std::nullopt;
		const bv& l = o.value();
		switch (nt) {
		case tau::wff_neg: return make_term_not(l);
		case tau::bf_neg: return make_bitvector_not(l);
		case tau::bf_cast: {
			size_t target_size = get_bv_size<node>(
				tau::get(n).get_ba_type_tree());
			size_t src_size = l.getSort().getBitVectorSize();
			if (target_size > src_size)
				return make_bitvector_zero_extend(
					l, target_size - src_size);
			if (target_size < src_size)
				return make_bitvector_extract(
					l, target_size - 1, 0);
			return o;
		}
		default:
			// kind_of said unary but no case here: the two tables
			// have drifted, and nullopt reads as "untranslatable"
			DBG(assert(false);)
			return std::nullopt;
		}
	};

	auto combine2 = [&](size_t nt, const std::optional<bv>& a,
		const std::optional<bv>& b) -> std::optional<bv>
	{
		if (!a || !b) return std::nullopt;
		const bv& l = a.value();
		const bv& r = b.value();
		switch (nt) {
		case tau::wff_and: return make_term_and(l, r);
		case tau::wff_or: return make_term_or(l, r);
		case tau::bf_eq: return make_term_equal(l, r);
		case tau::bf_neq: return make_term_distinct(l, r);
		case tau::bf_lteq: return make_term_less_equal(l, r);
		case tau::bf_nlteq: return make_term_not(make_term_less_equal(l, r));
		case tau::bf_gt: return make_term_greater(l, r);
		case tau::bf_ngt: return make_term_not(make_term_greater(l, r));
		case tau::bf_gteq: return make_term_greater_equal(l, r);
		case tau::bf_ngteq: return make_term_not(make_term_greater_equal(l, r));
		case tau::bf_lt: return make_term_less(l, r);
		case tau::bf_nlt: return make_term_not(make_term_less(l, r));
		case tau::bf_add: return make_bitvector_add(l, r);
		case tau::bf_sub: return make_bitvector_sub(l, r);
		case tau::bf_mul: return make_bitvector_mul(l, r);
		case tau::bf_div: return make_bitvector_div(l, r);
		case tau::bf_mod: return make_bitvector_mod(l, r);
		case tau::bf_and: return make_bitvector_and(l, r);
		case tau::bf_nand: return make_bitvector_nand(l, r);
		case tau::bf_or: return make_bitvector_or(l, r);
		case tau::bf_nor: return make_bitvector_nor(l, r);
		case tau::bf_xor: return make_bitvector_xor(l, r);
		case tau::bf_xnor: return make_bitvector_xnor(l, r);
		case tau::bf_shl: return make_bitvector_shl(l, r);
		case tau::bf_shr: return make_bitvector_shr(l, r);
		case tau::bf_min: return make_bitvector_min(l, r);
		case tau::bf_max: return make_bitvector_max(l, r);
		default:
			// see combine1: kind_of and this switch must agree
			DBG(assert(false);)
			return std::nullopt;
		}
	};

	// One entry per binder currently open, innermost last.
	struct binding {
		tref var;
		bv x;
		std::optional<bv> outer;	// shadowed binding, restored on up
	};

	std::vector<std::optional<bv>> vals;	// one value per finished node
	std::vector<size_t> ctxs{ ctx };	// innermost binder context
	std::vector<binding> binders;
	vals.reserve(64);

	auto memoise = [&](tref n, size_t c, const std::optional<bv>& v) {
		// looked up fresh rather than held across the children:
		// evaluating them inserts into memo and can rehash it
		if (v) memo[n].try_emplace(c, v.value());
	};

	auto down = [&](tref n) {
		if (auto mit = memo.find(n); mit != memo.end())
			if (auto it = mit->second.find(ctxs.back());
				it != mit->second.end())
			{
				vals.push_back(it->second);
				return false;	// already known, and no up
			}
		const size_t nt = tau::get(n).value.nt;
		if (kind_of(nt) != binder) {
			if (kind_of(nt) != leaf) return true;
			auto v = eval_leaf(n, nt);
			memoise(n, ctxs.back(), v);
			vals.push_back(std::move(v));
			return false;
		}
		tref v = tau::get(n).first();
		// a bound "variable" that is not one (variable capture in
		// substitution) binds nothing; the body is taken as it is
		if (!is<node>(v, tau::variable)) return true;
		size_t bv_size = get_bv_size<node>(
			tau::get(v).get_ba_type_tree());
		bv x = make_bitvector_var(
			cvc5_term_manager.mkBitVectorSort(bv_size),
			tau::get(v).to_str());
		// vars is shared, so an outer binding of the same tref (nested
		// quantifiers sharing a variable tref through caching) is saved
		// and restored rather than erased, which would drop it for the
		// caller
		auto prev = vars.find(v);
		binders.push_back({ v, x, prev != vars.end()
			? std::optional<bv>(prev->second) : std::nullopt });
		vars[v] = x;
		// a fresh, never-reused context: this binder is entered at most
		// once, so every shared subtree under it sees this same vars
		ctxs.push_back(++ctx_counter);
		return true;
	};

	auto up = [&](tref n) {
		const size_t nt = tau::get(n).value.nt;
		const kind k = kind_of(nt);
		// every visited child left exactly one value behind
		const size_t cs = tau::get(n).children_size();
		DBG(assert(vals.size() >= cs);)
		const size_t base = vals.size() - cs;
		std::optional<bv> res;
		switch (k) {
		case wrap: res = std::move(vals[base]); break;
		case unary: res = combine1(n, nt, vals[base]); break;
		case binary: res = combine2(nt, vals[base], vals[base + 1]);
			break;
		case binder: {
			// the body is the second child; the first is the bound
			// variable, which resolves through vars to its own term
			const std::optional<bv>& body = vals[base + cs - 1];
			if (binders.empty() || binders.back().var
				!= tau::get(n).first())
			{
				// bound nothing (see down), so no quantifier
				res = body;
				break;
			}
			const binding b = binders.back();
			binders.pop_back();
			ctxs.pop_back();
			if (b.outer) vars[b.var] = b.outer.value();
			else vars.erase(b.var);
			if (body) res = nt == tau::wff_all
				? make_term_forall({ b.x }, body.value())
				: make_term_exists({ b.x }, body.value());
			break;
		}
		default: break;	// leaf: down returned false, no up
		}
		vals.resize(base);
		memoise(n, ctxs.back(), res);
		vals.push_back(std::move(res));
	};

	tref root = form | tt::ref;
	if (!root) return std::nullopt;
	auto all = [](tref) { return true; };
	pre_order<node>(root).visit(down, all, up);
	DBG(assert(vals.size() == 1);)
	return vals.empty() ? std::nullopt : vals.front();
}

template<NodeType node>
std::optional<bv> bv_eval_node(tref form, subtree_map<node, bv>& vars,
	subtree_map<node, bv>& free_vars) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto t = tt(form);
	return bv_eval_node(t, vars, free_vars);
}


/**
 * @brief Checks that the formula can be decided by the bitvector solver:
 * every variable must have an explicitly sized bitvector type, at least one
 * bv-typed variable or ba_constant must actually be present, and the formula
 * must contain no node kind `bv_eval_node` cannot translate. Mixed-type
 * formulas (e.g. with sbf or tau variables) cannot be translated to cvc5,
 * and a formula with no bv content at all (e.g. an already-resolved
 * T/F literal) is not this solver's to claim.
 *
 * Also rejects formulas carrying a non-bv-typed ba_constant (e.g. a `qlt`
 * constant like `{1/3}:qlt`): such a constant can appear in an otherwise
 * bv-only clause once its variable has already been substituted by a
 * concrete value (e.g. during interpretation), so checking only `variable`
 * nodes is not enough to catch the mixed-type case.
 *
 * A `variable` node is treated as an opaque leaf: its children are not
 * descended into. An io_var (`o1[t]`, `i1[t-3]`) is itself a `variable`
 * node whose children carry the time offset/shift bookkeeping, including
 * an inner, untyped `variable` node for the bare time symbol `t` -- that
 * bookkeeping is not a data variable and bv_eval_node never looks past the
 * outer variable's own type when translating it, so inspecting it here
 * would reject bv-only formulas over their own opaque type mismatch.
 *
 * @tparam node Node type
 * @param form The formula to check
 * @return true if the formula is within the translator's reach: all
 * variables/constants are (explicitly sized) bitvectors and at least one
 * bv-typed variable/constant was seen
 */
template <NodeType node>
bool is_bv_solvable_formula(tref form, bv_unsolvable_reason& reason) {
	using tau = tree<node>;
	using tt = tau::traverser;

	reason = bv_unsolvable_reason::ok;
	// Temporal operators have no cvc5 encoding; bail out before translating.
	// Left as `ok` (not one of the specific rejections above): unlike a
	// foreign-typed variable/constant, a temporal wrapper is not something
	// blasting needs to give up on -- blasting rewrites the atomic bv
	// predicates it finds regardless of what logical/temporal operators
	// wrap them, so routing based on this reason should still allow it.
	if (tau::get(form).find_top(is_temporal_quantifier<node>))
		return false;

	bool solvable = true;
	// A formula with no bv content at all satisfied every rejection below
	// vacuously and was declared solvable, so a plain constant such as `1`
	// was handed to cvc5, which cannot translate it -- printing
	// "Failed to translate the formula to cvc5: 1" before the (correct)
	// result, and making a working normalization look like it had failed.
	// Reported on issue 28's corrected script, `g[0](y) := 0.
	// g[n](y) := g[n-1](y)'. n g[5](1)`.
	bool has_bv = false;
	auto check = [&](tref n) {
		// Reject references. Checking variables alone was not enough: a
		// wff_ref's arguments are perfectly good bv-typed variables, so the
		// whole formula was declared solvable, and `bv_eval_node` then hit the
		// reference, returned nullopt and logged "Failed to translate the
		// formula to cvc5" -- once per resolve pass plus the final gate, four
		// times for a single normalize_non_temp -- each time after a
		// cvc5::Solver had been constructed and the tree walked. `ref` covers
		// both wff_ref and bf_ref: they are named alternatives of the same
		// `ref` nonterminal.
		//
		// Only `ref`, deliberately. It is tempting to reject every node kind
		// bv_eval_node's switch lacks a case for -- io_var and capture in
		// particular -- but the grammar makes that wrong: `variable =>
		// (uconst | io_var | var_name) [typed]`, so an io_var is always
		// *inside* a variable node and `case tau::variable` translates it by
		// name, which is exactly how every bv io_var formula gets solved. The
		// same holds for a capture used as an io_var offset. Rejecting them
		// costs both correctness and time: measured on
		// test_integration-satisfiability3, whose six cases are all bv[16]
		// over io_vars, adding io_var to this list turned 1.4s and all-pass
		// into a >1500s timeout with a wrong answer, because every scope then
		// missed the solver shortcut and went to blasting instead.
		if (is<node>(n, tau::ref)) {
			reason = bv_unsolvable_reason::has_unresolved_ref;
			return solvable = false;
		}
		if (is_bv_type_family<node>(tau::get(n).get_ba_type()))
			has_bv = true;
		if (is<node>(n, tau::variable)) {
			size_t t = tau::get(n).get_ba_type();
			if (!is_bv_type_family<node>(t)) {
				reason = bv_unsolvable_reason::non_bv_variable;
				return solvable = false;
			}
			// the solver requires an explicit bitwidth
			if (!(tt(tau::get(n).get_ba_type_tree()) | tau::type | tau::subtype)) {
				reason = bv_unsolvable_reason::missing_bitwidth;
				return solvable = false;
			}
		} else if (is<node>(n, tau::ba_constant)) {
			// A non-bv-typed constant (e.g. a qlt constant like
			// `{1/3}:qlt`) can appear in an otherwise bv-only clause once
			// its variable has already been substituted by a concrete
			// value (e.g. during interpretation); checking only
			// `variable` nodes above is not enough to catch this
			// mixed-type case. Reused non_bv_variable: like a
			// foreign-typed variable, blasting has no arithmetic to
			// rewrite here either.
			size_t t = tau::get(n).get_ba_type();
			if (t != 0 && !is_bv_type_family<node>(t)) {
				reason = bv_unsolvable_reason::non_bv_variable;
				return solvable = false;
			}
		}
		return solvable;
	};
	// Do not descend into a variable node's own children: its offset/shift
	// bookkeeping (e.g. the untyped `t` inside an io_var) is not data.
	auto skip_variable_children = [](tref, tref parent = nullptr) {
		return !(parent && is<node>(parent, tau::variable));
	};
	auto up = [](tref) {};
	pre_order<node>(form).search_unique(check, skip_variable_children, up);
	if (solvable && !has_bv) reason = bv_unsolvable_reason::no_bv_content;
	return solvable && has_bv;
}

/** @copydoc is_bv_solvable_formula(tref,bv_unsolvable_reason&) */
template <NodeType node>
bool is_bv_solvable_formula(tref form) {
	bv_unsolvable_reason reason;
	return is_bv_solvable_formula<node>(form, reason);
}

/** @copydoc has_foreign_ba_constant */
template <NodeType node>
bool has_foreign_ba_constant(tref form) {
	using tau = tree<node>;

	auto foreign = [](tref n) {
		const tau& t = tau::get(n);
		if (!t.is_ba_constant() && !t.is(tau::bf_t) && !t.is(tau::bf_f))
			return false;
		const size_t ty = t.get_ba_type();
		return ty != 0 && !is_bv_type_family<node>(ty);
	};
	return tau::get(form).find_top(foreign) != nullptr;
}

/** @copydoc has_blasting_residue */
template <NodeType node>
bool has_blasting_residue(tref form) {
	using tau = tree<node>;

	// `bit_mask_cte` wraps the mask constant in a bf node, but the pipeline
	// trims such wrappers, so accept the constant at either depth.
	auto is_one_hot_mask = [](tref operand) -> bool {
		if (!operand) return false;
		tref c = tau::get(operand).is(tau::bf)
			? tau::trim(operand) : operand;
		if (!c || !tau::get(c).is_ba_constant()) return false;
		const auto& cte = tau::get(c).get_ba_constant();
		if (!std::holds_alternative<bv>(cte)) return false;
		const bv& term = std::get<bv>(cte);
		if (!term.isBitVectorValue()) return false;
		const std::string bits = term.getBitVectorValue();
		return std::ranges::count(bits, '1') == 1;
	};
	auto masking_conjunction = [&is_one_hot_mask](tref n) {
		if (!tau::get(n).is(tau::bf_and)) return false;
		return is_one_hot_mask(tau::get(n).first())
			|| is_one_hot_mask(tau::get(n).second());
	};
	return tau::get(form).find_top(masking_conjunction) != nullptr;
}

/**
 * @brief Does @p form contain a quantifier of one kind nested inside one of the
 * other kind?
 *
 * cvc5's default counterexample-guided instantiation only instantiates the
 * innermost quantified subformula, which collapses on interleaved `all`/`ex`
 * over bitvectors; `bv_formula_sat_status` turns that default off when this
 * predicate holds. A formula whose quantifiers all point the same way, or whose
 * two kinds sit in unrelated branches, does not need it.
 * @tparam node Tree node type.
 * @param form Formula to inspect.
 * @return `true` if some `wff_all` has a `wff_ex` below it, or vice versa.
 */
template <NodeType node>
bool has_alternating_quantifiers(tref form) {
	using tau = tree<node>;
	for (tref q : tau::get(form).select_all(is<node, tau::wff_all>))
		if (tau::get(q).find_top(is<node, tau::wff_ex>)) return true;
	for (tref q : tau::get(form).select_all(is<node, tau::wff_ex>))
		if (tau::get(q).find_top(is<node, tau::wff_all>)) return true;
	return false;
}

template <NodeType node>
std::optional<bv_sat_status> bv_formula_sat_status(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;

#ifdef TAU_CACHE
	// One cvc5::Solver construction plus one checkSat per call, and the callers
	// ask repeatedly: resolve_quantifiers is a whole-tree pre_order run at
	// least three times per eliminate_arithmetic_and_quantifiers, and its open-scope
	// branch asks twice per scope. The answer depends only on `form`: the one
	// global config_cvc5_solver reads, `cvc5_options`, is fixed at process
	// start before the first query (documented at its definition -- flipping
	// it mid-process would serve verdicts computed under the previous option
	// set), config_cvc5_solver_alternating_quantifiers reads no global at all,
	// and the alternation test is a function of the formula -- so the formula
	// alone is a complete key. Unlike anti_prenex's memo (which had to be
	// split per `preprocessing` setting) there is nothing else to key on.
	// nullopt is cached too: a formula the translator rejects gets rejected the
	// same way every time, and re-deriving that costs a full tree walk.
	using cache_t = std::unordered_map<tref, std::optional<bv_sat_status>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	tref key = tau::trim_right_sibling(form);
	if (auto it = cache.find(key); it != end(cache)) return it->second;
	auto memo = [&key](std::optional<bv_sat_status> r) {
		return cache.emplace(key, r).first->second;
	};
#else
	auto memo = [](std::optional<bv_sat_status> r) { return r; };
#endif // TAU_CACHE

	subtree_map<node, bv> vars, free_vars;
	// Opt-in quantifier-free decision (bv_quantifier_free_decision). A closed
	// formula whose binders are all existential, all in positive polarity and
	// each variable bound once, is satisfiable exactly when its matrix is, so
	// the binders are dropped and the variables become free constants; one
	// whose binders are all universal is satisfiable exactly when the negated
	// matrix is unsatisfiable, so it is negated as well and the verdict
	// inverted. Either way cvc5 then sees a QF_BV problem and bitblasts it
	// eagerly, instead of running its quantifier instantiation on a formula
	// that has no alternation to instantiate. Anything else -- both kinds,
	// a binder under a negation, a variable bound twice -- takes the path
	// below unchanged.
	if (bv_quantifier_free_decision_enabled()) {
		const bool has_ex = tau::get(form).find_top(is<node, tau::wff_ex>) != nullptr;
		const bool has_all = tau::get(form).find_top(is<node, tau::wff_all>) != nullptr;
		if (has_ex != has_all) {
			const auto kind = has_ex ? tau::wff_ex : tau::wff_all;
			// The universal identity sat(all x phi) == !sat(!phi) holds
			// only for a closed formula: with a free y, sat(all x phi(x, y))
			// asks for some y that works for every x, whereas !sat(!phi)
			// asks that phi hold for every x and every y. Every caller
			// closes the formula before asking, but the pass must not
			// depend on that, so a universal with a free variable takes
			// the quantified path. The existential identity has no such
			// condition: the free variables simply stay free.
			bool eligible = has_ex || get_free_vars<node>(form).empty();
			subtree_map<node, int> bound;
			std::vector<std::pair<tref, bool>> stack{{form, false}};
			while (!stack.empty() && eligible) {
				auto [n, under_neg] = stack.back(); stack.pop_back();
				if (!n) continue;
				const tau& t = tau::get(n);
				if (t.is(tau::wff_neg)) under_neg = true;
				if (t.is(kind)) {
					if (under_neg || !is<node>(t.first(), tau::variable)
						|| ++bound[t.first()] > 1) eligible = false;
				}
				for (tref c : t.children()) stack.push_back({c, under_neg});
			}
			if (eligible) {
				auto drop_binder = [kind](tref n) -> tref {
					const tau& t = tau::get(n);
					if (t.is(tau::wff) && t.child_is(kind)) return t[0].second();
					return n;
				};
				tref matrix = form;
				for (;;) { // binders nest; peel until none is left
					tref next = pre_order<node>(matrix).apply_unique(drop_binder, while_is_formula<node>);
					if (next == matrix) break;
					matrix = next;
				}
				const bool invert = !has_ex;
				if (invert) matrix = tau::build_wff_neg(matrix);
				LOG_DEBUG << "bv_formula_sat_status: quantifier-free decision"
					<< (invert ? " (universal, inverted)" : "") << ": " << LOG_FM(matrix);
				cvc5::Solver qf_solver(cvc5_term_manager);
				config_cvc5_solver_quantifier_free(qf_solver);
				auto qf_expr = bv_eval_node<node>(tt(matrix), vars, free_vars);
				if (!qf_expr) {
					LOG_ERROR << "Failed to translate the formula to cvc5: " << LOG_FM(matrix);
					return memo(std::nullopt);
				}
				qf_solver.assertFormula(qf_expr.value());
				auto qf_result = qf_solver.checkSat();
				if (qf_result.isSat()) return memo(invert ? bv_sat_status::unsat : bv_sat_status::sat);
				if (qf_result.isUnknown()) {
					LOG_DEBUG << "cvc5 could not decide satisfiability (unknown) for: " << qf_expr.value();
					return memo(bv_sat_status::unknown);
				}
				return memo(invert ? bv_sat_status::sat : bv_sat_status::unsat);
			}
		}
	}
	// A fresh solver per query is deliberate, do NOT share one like
	// normalize_bv's (B12): cvc5 forbids a second checkSat without
	// incremental mode ("cannot make multiple queries unless incremental
	// solving is enabled" -- resetAssertions does not lift this), and a
	// pair of long-lived incremental solvers measured strictly worse on
	// bv[64]x14 stress: 19.4s -> 30.0s wall and 227MB -> 1.1GB peak RSS
	// (2026-08-17). The engine construction cost per query is the price of
	// the non-incremental option set, which is the larger win.
	cvc5::Solver solver(cvc5_term_manager);
	// Interleaved all/ex over bitvectors needs cvc5 to instantiate outer
	// quantifiers too, not just the innermost one; without that it does not
	// finish at all past bv[8]. Applied before the logic is fixed, since
	// cvc5 resolves its quantifier-module defaults at that point.
	// See config_cvc5_solver_alternating_quantifiers for the measurements.
	// Genuine alternation, not merely "both kinds occur somewhere": a
	// quantifier of one kind must sit *below* one of the other kind. Testing
	// find_top(wff_all) && find_top(wff_ex) also matched non-alternating
	// shapes such as `(ex x P(x)) && (all y Q(y))`, which then paid the
	// strategy change's cost (up to ~1.8x on quantified division, see
	// config_cvc5_solver_alternating_quantifiers) for no benefit.
	if (has_alternating_quantifiers<node>(form))
		config_cvc5_solver_alternating_quantifiers(solver);
	// decision_only: this function only ever reads the checkSat verdict,
	// never a model, so satisfiability-preserving preprocessing is admissible
	// here (see cvc5_option_set::decision_no_models).
	config_cvc5_solver(solver, true);

	auto expr = bv_eval_node<node>(tt(form), vars, free_vars);
	if (!expr) {
		LOG_DEBUG << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		DBG(LOG_TRACE << LOG_FM_TREE(form) << "\n";)
		return memo(std::nullopt);
	}
	DBG( LOG_TRACE << "CVC5 translated formula: " << expr.value(); )
	solver.assertFormula(expr.value());
	auto result = solver.checkSat();
	if (result.isSat()) return memo(bv_sat_status::sat);
	if (result.isUnknown()) {
		LOG_DEBUG << "cvc5 could not decide satisfiability (unknown) for: " << expr.value();
		return memo(bv_sat_status::unknown);
	}
	return memo(bv_sat_status::unsat);
}

template <NodeType node>
bool is_bv_formula_sat(tref form) {
	// Collapses unknown and translation failure into false, same as unsat.
	// Callers that would otherwise assert the formula is definitely false
	// on a false return here must use bv_formula_sat_status instead and
	// treat unknown/nullopt as "cannot decide".
	return bv_formula_sat_status<node>(form) == bv_sat_status::sat;
}

template <NodeType node>
bool is_bv_formula_unsat(tref form) {
	return !is_bv_formula_sat<node>(form);
}

template <NodeType node>
bool is_bv_formula_valid(tref form) {
	using tau = tree<node>;
	return is_bv_formula_unsat<node>(tau::build_wff_neg(form));
}

template <NodeType node>
std::optional<solution<node>> solve_bv(const tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;

	subtree_map<node, bv> vars, free_vars;
	// Fresh solver per query, same rationale as bv_formula_sat_status above:
	// checkSat() is not safe to replay on a shared/reset solver instance.
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	auto expr = bv_eval_node<node>(tt(form), vars, free_vars);
	if (!expr) {
		LOG_DEBUG << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		DBG(LOG_TRACE << LOG_FM_TREE(form) << "\n";)
		return std::nullopt;
	}
	DBG( LOG_TRACE << "CVC5 translated formula: " << expr.value(); )

	solver.assertFormula(expr.value());
	LOG_DEBUG << "Solving bitvector formula: " << expr.value();
	auto result = solver.checkSat();
	// extract the model and return the solution if sat
	if (result.isSat()) {
		LOG_DEBUG << "Bitvector system is sat.";
		solution<node> s;
		for (const auto& [tau_var, bv_var] : free_vars) {
			bv cte = solver.getValue(bv_var);
			s.emplace(tau::get(tau::bf, tau_var),
				tau::get(tau::bf, tau::get_ba_constant(cte,
					bv_type<node>(cte.getSort().getBitVectorSize()))));
		}
		return s;
	}
	// Callers of this overload (solve_bv(trefs) -> solver.tmpl.h) already
	// treat "no solution" uniformly as "skip this clause" regardless of
	// the reason, which is sound for unknown as well as unsat (neither
	// asserts a definite truth value), so nullopt is returned for both;
	// only the diagnostic differs.
	if (result.isUnknown())
		LOG_DEBUG << "cvc5 could not decide satisfiability (unknown) for: " << expr.value();
	else
		LOG_DEBUG << "Bitvector system is unsat.";
	return {};
}

template<NodeType node>
std::optional<solution<node>> solve_bv(const trefs& lits) {
	using tau = tree<node>;

	return solve_bv<node>(tau::build_wff_and(lits));
}

} // namespace idni::tau_lang