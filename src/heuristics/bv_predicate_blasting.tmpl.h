// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

//#include "bv_predicate_blasting_predicates.tmpl.h"

namespace idni::tau_lang {

/**
 * @brief Extracts the arguments of a bitvector multiplication term.
 *
 * If one child is a bitvector constant, returns (non-constant, constant).
 * Otherwise, returns (nullptr, nullptr).
 *
 * @tparam node Node type
 * @param term The term to analyze
 * @return Pair of (non-constant, constant) or (nullptr, nullptr)
 *
 * @par Example
 * @code{.cpp}
 * // "x * { 6 }:bv[4]" -> (x, { 6 }:bv[4])
 * tref fm = get_nso_rr("x * { 6 }:bv[4] = 0.").value().main->get();
 * tref mul_term = tau::trim(tau::get(fm)[0].first());
 * auto [var, cst] = get_bvmul_arguments<node_t>(mul_term);
 * // tau::get(var).to_str() == "x", tau::get(cst).to_str() == "{ 6 }:bv[4]"
 * @endcode
 */
template <NodeType node>
static std::pair<tref, tref> get_bvmul_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	auto tl = tau::trim(left), tr = tau::trim(right);
	if (tau::get(tl).is_ba_constant() && is_bv_constant<node>(tl)) {
		return std::make_pair(right, left);
	}
	if (tau::get(tr).is_ba_constant() && is_bv_constant<node>(tr)) {
		return std::make_pair(left, right);
	}

	// None of the factors is constant, we cannot blast this multiplication.
	return std::make_pair(nullptr, nullptr);
}

/**
 * @brief Extracts the arguments of a bitvector division/modulo or shift term.
 *
 * If the right child is a bitvector constant, returns (left, right).
 * Otherwise, returns (nullptr, nullptr).
 *
 * @tparam node Node type
 * @param term The term to analyze
 * @return Pair of (dividend, divisor) or (nullptr, nullptr)
 *
 * @par Example
 * @code{.cpp}
 * // "x / { 3 }:bv[4]" -> (x, { 3 }:bv[4])
 * tref fm = get_nso_rr("x / { 3 }:bv[4] = 0.").value().main->get();
 * tref div_term = tau::trim(tau::get(fm)[0].first());
 * auto [dividend, divisor] = get_arguments<node_t>(div_term);
 * // tau::get(dividend).to_str() == "x", tau::get(divisor).to_str() == "{ 3 }:bv[4]"
 * @endcode
 */
template <NodeType node>
static std::pair<tref, tref> get_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	auto tr = tau::trim(right);
	if (tau::get(tr).is_ba_constant() && is_bv_constant<node>(tr)) {
		return std::make_pair(left, right);
	}

	// Divisor/modulo/count is not constant, we cannot blast this division/modulo/shift.
	return std::make_pair(nullptr, nullptr);
}

// Memo for bit_slice, shared across one quantify_aux_vars elimination
// attempt: keyed by (term, bit index).
template<NodeType node>
using bit_slice_memo = std::unordered_map<std::pair<tref, size_t>,
	std::optional<typename tau_term_bdd<node>::ref>>;

/**
 * @brief Builds the BDD for a blasted bf term's bit-@p i, under order @p o.
 *
 * Recurses structurally over bf_and/bf_or/bf_xor/bf_neg -- all bitwise, so
 * bit i of the combination is the bdd_and/bdd_or/bdd_xor-via-bdd_and-or/
 * bdd_not of bit i of each operand. At a bv variable leaf, the decision
 * variable is bit<node>(V, i) -- the same masked term blasted atoms already
 * build, so hash-consing lines it up with existing atoms and with the order
 * @p o registers under (see quantify_aux_vars) -- looked up directly and
 * built via from_bit, deliberately NOT via build_bdd(term, o): build_bdd's
 * bf_and/bf_or cases flatten through get_cnf_bf_clauses/get_dnf_bf_clauses,
 * a purely structural walk that does not consult @p o, so re-embedding a
 * registered bit(V,i) leaf (itself a bf_and node) as a plain subterm of
 * another reconstructed bf_and/bf_or would let that flattening shatter it
 * into its raw (unregistered) variable and mask constant. Combining BDD
 * refs directly, one leaf at a time, avoids ever re-emitting such a term.
 * At a bv constant leaf, evaluates bit i directly to the T/F terminal.
 * Declines (returns std::nullopt) on anything else -- casts, unresolved
 * recurrence calls, non-bv content -- rather than guess.
 *
 * Memoized by (t, i): a bvadd/bvsub accumulator's xor chain is shared
 * across every bit position, so naive re-slicing is O(width) per bit,
 * O(width^2) per atom, times O(width) atoms sharing the accumulator --
 * O(width^3) without this.
 *
 * @tparam node Node type
 * @param t The (bf-wrapped) term to slice
 * @param i Bit index (0 = least significant)
 * @param o Bit-level variable order (see quantify_aux_vars)
 * @param memo (t, i) -> BDD memo
 * @return The bit-i BDD, or std::nullopt if t cannot be sliced
 */
template<NodeType node>
static std::optional<typename tau_term_bdd<node>::ref> bit_slice(tref t,
	size_t i, const typename term_handle<node>::order& o,
	bit_slice_memo<node>& memo)
{
	using tau = tree<node>;
	using tbdd = tau_term_bdd<node>;
	using ref = typename tbdd::ref;

	if (const auto it = memo.find(std::pair{t, i}); it != memo.end())
		return it->second;
	std::optional<ref> res;
	switch (tau::get(t).get_type()) {
		case tau::bf: res = bit_slice<node>(tau::trim(t), i, o, memo); break;
		case tau::bf_and: case tau::bf_or: case tau::bf_xor: {
			const tau& tt = tau::get(t);
			auto l = bit_slice<node>(tt.first(), i, o, memo);
			auto r = l ? bit_slice<node>(tt.second(), i, o, memo)
				: std::nullopt;
			if (!l || !r) break;
			if (tt.is(tau::bf_and)) res = tbdd::bdd_and(*l, *r, o);
			else if (tt.is(tau::bf_or)) res = tbdd::bdd_or(*l, *r, o);
			else res = tbdd::bdd_or(
				tbdd::bdd_and(*l, tbdd::bdd_not(*r), o),
				tbdd::bdd_and(tbdd::bdd_not(*l), *r, o), o);
			break;
		}
		case tau::bf_neg: {
			auto c = bit_slice<node>(tau::get(t).first(), i, o, memo);
			if (c) res = tbdd::bdd_not(*c);
			break;
		}
		case tau::variable: {
			tref leaf = tau::trim(bit<node>(tau::get(tau::bf, t),
				static_cast<int_t>(i)));
			if (o.find(leaf) != o.end()) res = tbdd::from_bit(leaf);
			break;
		}
		case tau::ba_constant: {
			if (!is_bv_constant<node>(t)) break;
			auto cte = std::get<bv>(tau::get(t).get_ba_constant());
			// A constant folded from a bf_and of two ba_constants (e.g. a
			// mask AND a literal operand, as bit<node> builds for a
			// constant leaf) can come back from the hooks as an
			// un-evaluated cvc5 Term (e.g. "(bvand #b0101 #b0010)")
			// instead of a literal -- normalize_bv (bv_ba.h) runs cvc5's
			// own simplifier to fold it down before giving up.
			if (!cte.isBitVectorValue()) cte = normalize_bv(cte);
			if (!cte.isBitVectorValue()) break;
			std::string bv_str = cte.getBitVectorValue();
			if (i >= bv_str.size()) break;
			res = (bv_str[bv_str.size() - 1 - i] == '1') ? tbdd::T : tbdd::F;
			break;
		}
		default: break;
	}
	memo.emplace(std::pair{t, i}, res);
	return res;
}

/**
 * @brief Builds a BDD for a blasted WFF whose atoms are bv (dis)equalities.
 *
 * wff_and/wff_or combine via the existing CNF/DNF clause extractors and
 * bdd_and/bdd_or; wff_neg via bdd_not; wff_equiv via
 * bdd_ite(A, B, not B); wff_t/wff_f map to the BDD terminals directly. An
 * atom bf_eq(t1, t2) (which also covers bf_eq_0, always represented as
 * bf_eq(t, 0)) becomes an AND, over every bit i, of NOT(BDD of
 * bit_slice(t1 xor t2, i)) -- "bit i of t1 xor t2 is zero"; bf_neq is the
 * same, negated.
 *
 * Declines (returns std::nullopt) on any atom that is not a bv-typed
 * (dis)equality -- bf_lt/bf_gt/... (div/mod's side conditions; out of v1
 * scope) -- or on any wff node this function does not recognise.
 *
 * @tparam node Node type
 * @param f The (wff-wrapped) formula to build a BDD for
 * @param o Bit-level variable order (see quantify_aux_vars)
 * @param memo bit_slice's memo, shared across the whole call
 * @return The BDD reference, or std::nullopt on decline
 */
template<NodeType node>
static std::optional<typename tau_term_bdd<node>::ref> build_wff_bdd(tref f,
	const typename term_handle<node>::order& o, bit_slice_memo<node>& memo)
{
	using tau = tree<node>;
	using tbdd = tau_term_bdd<node>;
	using ref = typename tbdd::ref;

	// bitwidth-many negated bit-BDDs ANDed via bdd_and_many rather than
	// folded pairwise with bdd_and: bdd_and_many does a proper multi-way
	// Shannon split (smallest variable across ALL operands at once, with
	// subsumption elimination), which stays tractable in TAU_CACHE-less
	// Debug builds where pairwise bdd_and has no memoization to fall
	// back on.
	auto zero_test = [&](tref t1, tref t2) -> std::optional<ref> {
		if (!is_bv_type_family<node>(tau::get(t1).get_ba_type()))
			return std::nullopt;
		size_t bitwidth = get_bv_type_bitwidth<node>(t1);
		if (bitwidth == 0) return std::nullopt;
		tref x = tau::build_bf_xor(t1, t2);
		typename tbdd::refs bits;
		bits.reserve(bitwidth);
		for (size_t i = 0; i < bitwidth; ++i) {
			auto s = bit_slice<node>(x, i, o, memo);
			if (!s) return std::nullopt;
			bits.push_back(tbdd::bdd_not(*s));
		}
		return tbdd::bdd_and_many(std::move(bits), o);
	};

	switch (tau::get(f).get_type()) {
		case tau::wff: return build_wff_bdd<node>(tau::trim(f), o, memo);
		case tau::wff_t: return tbdd::T;
		case tau::wff_f: return tbdd::F;
		case tau::wff_and: {
			typename tbdd::refs clauses;
			for (tref c : get_cnf_wff_clauses<node>(f)) {
				auto r = build_wff_bdd<node>(c, o, memo);
				if (!r) return std::nullopt;
				clauses.push_back(*r);
			}
			return tbdd::bdd_and_many(std::move(clauses), o);
		}
		case tau::wff_or: {
			typename tbdd::refs clauses;
			for (tref d : get_dnf_wff_clauses<node>(f)) {
				auto r = build_wff_bdd<node>(d, o, memo);
				if (!r) return std::nullopt;
				clauses.push_back(*r);
			}
			return tbdd::bdd_or_many(std::move(clauses), o);
		}
		case tau::wff_neg: {
			auto r = build_wff_bdd<node>(tau::get(f).first(), o, memo);
			if (!r) return std::nullopt;
			return tbdd::bdd_not(*r);
		}
		case tau::wff_equiv: {
			const tau& tf = tau::get(f);
			auto a = build_wff_bdd<node>(tf.first(), o, memo);
			if (!a) return std::nullopt;
			auto b = build_wff_bdd<node>(tf.second(), o, memo);
			if (!b) return std::nullopt;
			return tbdd::bdd_ite(*a, *b, tbdd::bdd_not(*b), o);
		}
		case tau::bf_eq: {
			const tau& tf = tau::get(f);
			return zero_test(tf.first(), tf.second());
		}
		case tau::bf_neq: {
			const tau& tf = tau::get(f);
			auto r = zero_test(tf.first(), tf.second());
			if (!r) return std::nullopt;
			return tbdd::bdd_not(*r);
		}
		default: return std::nullopt;
	}
}

/**
 * @brief Converts a post-elimination BDD back to a WFF (not a term).
 *
 * IMPORTANT: this is deliberately not to_tau_term wrapped as `= 0`. That
 * would be unsound here: to_tau_term conjoins masked leaf terms at the term
 * level, and masked terms of different bit positions have disjoint masks,
 * so their term-AND is identically zero. (Sanity check: phi = "bit 0 of V
 * is 1"; the term route yields bit(V,0) wrapped `= 0`, asserting the
 * opposite.) This instead rebuilds the disjunction of guarded branches at
 * the formula level: for a decision on leaf v = bit(V, i) with branches
 * high/low, ( !(v=0) && high ) || ( (v=0) && low ), using the same
 * zero-test atom shape blasting already builds elsewhere (is_bit_zero /
 * is_bit_one). Memoized by BDD ref.
 *
 * @tparam node Node type
 * @param x BDD reference to convert
 * @param memo BDD ref -> wff memo
 * @return The equivalent wff
 */
template<NodeType node>
static tref bdd_to_wff(typename tau_term_bdd<node>::ref x,
	std::unordered_map<typename tau_term_bdd<node>::ref, tref>& memo)
{
	using tau = tree<node>;
	using tbdd = tau_term_bdd<node>;

	if (x == tbdd::T) return _T<node>();
	if (x == tbdd::F) return _F<node>();
	if (const auto it = memo.find(x); it != memo.end()) return it->second;

	tref v = tau::get(tau::bf, tbdd::get_var(x));
	tref hw = bdd_to_wff<node>(tbdd::get_high(x), memo);
	tref lw = bdd_to_wff<node>(tbdd::get_low(x), memo);

	tref pos = tau::build_wff_neg(tau::build_bf_eq_0(v)); // is_bit_one shape
	tref neg = tau::build_bf_eq_0(v);                     // is_bit_zero shape
	const tau& hwt = tau::get(hw);
	const tau& lwt = tau::get(lw);
	tref res;
	if (hwt.equals_T() && lwt.equals_F()) res = pos;
	else if (hwt.equals_F() && lwt.equals_T()) res = neg;
	else {
		tref hi = hwt.equals_F() ? _F<node>()
			: hwt.equals_T() ? pos : tau::build_wff_and(pos, hw);
		tref lo = lwt.equals_F() ? _F<node>()
			: lwt.equals_T() ? neg : tau::build_wff_and(neg, lw);
		res = tau::get(hi).equals_F() ? lo
			: tau::get(lo).equals_F() ? hi
			: tau::build_wff_or(hi, lo);
	}
	memo.emplace(x, res);
	return res;
}

/**
 * @brief Counts the unique BDD nodes reachable from @p x.
 *
 * Used as quantify_aux_vars's defensive budget check: a cheap, meaningful
 * proxy for "how big has the running BDD gotten" (as opposed to counting
 * abstract bdd_and/bdd_ex calls, which says little about actual size when
 * TAU_CACHE is off and calls can recurse without memoization).
 *
 * @tparam node Node type
 * @param x BDD reference to measure
 * @param seen Visited-node set (dedupes shared sub-BDDs); pass a fresh one
 *        per top-level call
 * @return Number of unique non-terminal nodes reachable from x
 */
template<NodeType node>
static size_t bdd_node_count(typename tau_term_bdd<node>::ref x,
	std::unordered_set<typename tau_term_bdd<node>::ref>& seen)
{
	using tbdd = tau_term_bdd<node>;
	if (x == tbdd::T || x == tbdd::F) return 0;
	if (!seen.insert(x).second) return 0;
	return 1 + bdd_node_count<node>(tbdd::get_high(x), seen)
		 + bdd_node_count<node>(tbdd::get_low(x), seen);
}

/**
 * @brief Collects the decision variables (bit(V,i) trefs) reachable from a
 * BDD -- its support.
 *
 * Used by quantify_aux_vars to schedule per-BIT early quantification: a
 * plain visited-node walk, same traversal shape as bdd_node_count, deduping
 * shared sub-BDDs so a diamond-shaped BDD is still linear, not exponential.
 * BDDs here are small (one blasted conjunct at a time), so this stays cheap.
 *
 * @tparam node Node type
 * @param x BDD reference to walk
 * @param support Output set of decision-variable trefs (accumulated)
 * @param seen Visited-node set (dedupes shared sub-BDDs); pass a fresh one
 *        per top-level call
 */
template<NodeType node>
static void collect_bdd_support(typename tau_term_bdd<node>::ref x,
	subtree_set<node>& support,
	std::unordered_set<typename tau_term_bdd<node>::ref>& seen)
{
	using tbdd = tau_term_bdd<node>;
	if (x == tbdd::T || x == tbdd::F) return;
	if (!seen.insert(x).second) return;
	support.insert(tbdd::get_var(x));
	collect_bdd_support<node>(tbdd::get_high(x), support, seen);
	collect_bdd_support<node>(tbdd::get_low(x), support, seen);
}

/**
 * @brief Existentially quantifies the auxiliary variables introduced during
 * blasting on top of subformula.
 *
 * Unlike build_wff_ex_many, the quantifier ids are chosen above any numeric
 * variable occurring in subformula -- bound or free. Operands may contain
 * variables bound by quantifiers outside the atomic being blasted (already
 * renamed to their canonical numeric form), and reusing such an id here
 * would capture them.
 *
 * @tparam node Node type
 * @param vars The auxiliary variables to quantify (innermost last)
 * @param subformula The formula to quantify over
 * @return The quantified formula
 *
 * @par Example
 * This is internal plumbing shared by every `*_predicate` function
 * (`eq_predicate`, `lt_predicate`, ...) and by `bvadd`/`bvsub`/`bvmul`/
 * `bvdiv`/`bvmod`/`bved` to close over the fresh carry/borrow/quotient-style
 * variables their constraints introduce. Conceptually: given `vars = [c0,
 * c1]` (two fresh carry variables) and a `subformula` whose highest existing
 * numeric variable id is, say, `5`, this wraps `subformula` in `ex c1 (ex c0
 * (subformula[c0 -> "6", c1 -> "7"]))`, then immediately eliminates both
 * quantifiers via `resolve_quantifiers2` (BDD-based, since blasted
 * arithmetic commonly introduces many disjuncts) rather than leaving them
 * in the result. See @ref bvadd's example: the two carry variables its
 * bit-recurrence introduces are exactly the kind of `vars` this function
 * closes over.
 */
template<NodeType node>
static tref quantify_aux_vars(const trefs& vars, tref subformula) {
	using tau = tree<node>;

	if (vars.empty()) return subformula;
	int_t id = find_biggest_quant_id<node>(subformula);
	auto is_number = [](const std::string& s) {
		if (s.empty()) return false;
		for (const unsigned char c : s)
			if (!std::isdigit(c)) return false;
		return true;
	};
	auto f = [&](tref n) {
		if (is<node, tau::variable>(n)) {
			// HE-18: a user variable literally named e.g.
			// 9999999999 must not throw out of the blasting pass;
			// skip anything that does not fit.
			if (const auto& name = get_var_name<node>(n);
				is_number(name)) try {
				id = std::max(id, (int_t)std::stoll(name));
			} catch (const std::out_of_range&) { /* skip */ }
		}
		return true;
	};
	pre_order<node>(subformula).visit_unique(f);
	subtree_map<node, tref> changes;
	tref res = subformula;
	for (tref v : vars | std::views::reverse) {
		changes.emplace(v, tau::build_variable(std::to_string(++id),
			tau::get(v).get_ba_type()));
		res = tau::build_wff_ex(v, res, false);
	}
	res = rewriter::replace<node>(res, changes);
	// The aux variables just wrapped (carries/borrows/partial products/...)
	// retain a bv BA-type tag but are ordinary Boole-decomposable content --
	// always eliminate/push them here, regardless of nominal bv typing.
	// Scoped to exactly this local wrapping (not the whole formula being
	// blasted), so a still-unresolved, genuinely bv-typed quantifier
	// elsewhere (e.g. a user-level variable `x` still appearing in an
	// unblasted comparison atom like `x = 3`, since it has no arithmetic
	// to blast) is not caught up in it.
	//
	// Uses resolve_quantifiers2's BDD-based elimination directly, not
	// anti_prenex_block's generic Boole decomposition: blasted arithmetic
	// (e.g. XOR-expanded sum/carry constraints) commonly introduces
	// disjunctions and many atoms, and naive atom-by-atom Boole
	// decomposition scales combinatorially with atom count where BDD
	// elimination does not.
	typename term_handle<node>::order ord;
	for (size_t i = 0; i < vars.size(); ++i)
		ord.emplace(changes[vars[vars.size() - 1 - i]],
			static_cast<int_t>(i));
	tref result = resolve_quantifiers2<node>(res, ord, eliminability<node>::none());

	// resolve_quantifiers2's fast path only handles a single-atom body;
	// blasted arithmetic's conjunction of carry/definition constraints
	// doesn't match that shape and is returned unchanged, leaving these
	// aux quantifiers stuck. Detect that (any renamed aux variable still
	// wff_ex-bound in the result) and fall back to bit-level BDD
	// elimination: sound because bv[N] is a product of N 2-valued BAs, so
	// eliminating a bv variable is exactly eliminating its bits
	// independently, each a genuine Boolean (Shannon) existential over
	// the registered tau_term_bdd engine.
	subtree_set<node> aux_images;
	for (tref v : vars) aux_images.insert(changes[v]);
	bool stuck = false;
	auto has_stuck_aux = [&](tref m) {
		if (is_child<node>(m, tau::wff_ex) && aux_images.contains(tau::trim2(m)))
			stuck = true;
		return true;
	};
	pre_order<node>(result).visit_unique(has_stuck_aux);
	if (!stuck) return result;

	// Collect every bv-typed variable occurring in the renamed body (aux
	// images and any surviving free variables alike) and register each
	// one's bits as BDD decision variables, bit-major/variable-minor (all
	// vars' bit i adjacent), so a variable narrower than the widest one
	// present is simply absent from the higher ranks.
	tref body = res;
	for (size_t i = 0; i < vars.size(); ++i) body = tau::get(body)[0].second();
	trefs occurring;
	subtree_set<node> seen;
	auto collect_vars = [&](tref m) {
		if (is<node, tau::variable>(m)
			&& is_bv_type_family<node>(tau::get(m).get_ba_type())
			&& seen.insert(m).second)
			occurring.push_back(m);
		return true;
	};
	pre_order<node>(body).visit_unique(collect_vars);
	std::vector<std::pair<tref, size_t>> widths;
	size_t max_bitwidth = 0;
	for (tref w : occurring) {
		size_t bw = get_bv_type_bitwidth<node>(w);
		if (bw == 0) return result; // not actually bv-typed; bail out safely
		widths.emplace_back(w, bw);
		max_bitwidth = std::max(max_bitwidth, bw);
	}
	typename term_handle<node>::order bit_ord;
	subtree_set<node> aux_bits;
	subtree_unordered_map<node, tref> bit_owner;
	int_t rank = 0;
	for (size_t i = 0; i < max_bitwidth; ++i)
		for (const auto& [w, bw] : widths)
			if (i < bw) {
				tref b = tau::trim(bit<node>(tau::get(tau::bf, w), (int_t)i));
				bit_ord.emplace(b, rank++);
				bit_owner.emplace(b, w);
				if (aux_images.contains(w)) aux_bits.insert(b);
			}

	// Early quantification, at per-BIT granularity: fold the top-level
	// conjuncts in batches (see the batching loop below) instead of
	// building one BDD for the whole (potentially large) conjunction and
	// quantifying everything at the end. Each aux BIT (not whole aux
	// variable) is existentially closed the moment the last conjunct
	// reaching it has been folded in -- sound because pushing an
	// existential past conjuncts whose BDD does not depend on the bound
	// decision variable is a standard equivalence, and each bit(V,i) is
	// itself an independent Boolean (Shannon) decision variable of the
	// registered tau_term_bdd engine. This is what makes chained/nested
	// arithmetic tractable: without it, the whole body's BDD (spanning
	// every carry/borrow bit of every chained bvadd/bvsub at once) blows
	// up.
	//
	// Per-bit (not per-variable) granularity matters: bit i of a
	// bvadd/bvsub carry/borrow variable is only reached by a couple of
	// adjacent conjuncts (ripple structure: conjunct k's BDD touches bits
	// ~k and k+1), so each bit can be closed shortly after its last use,
	// keeping the live BDD frontier short.
	//
	// Conjuncts' BDDs are built once, upfront (bit_ord is already fixed),
	// so their actual support -- the decision variables they reach -- is
	// known before conjuncts are ordered into batches. Order: sorted by
	// the HIGHEST bit_ord rank in each conjunct's support, lowest rank as
	// a tie-break, original position as a final tie-break. Since bit_ord
	// assigns ranks bit-major (all variables' bit i before any variable's
	// bit i+1), sorting by highest support rank tracks chain adjacency: a
	// conjunct touching only bit i sorts near other bit-i/(i+1)
	// conjuncts, and a wide-spanning conjunct sorts after every conjunct
	// that could still shorten its support once folded in.
	trefs conjuncts = get_cnf_wff_clauses<node>(body);
	if (conjuncts.empty()) conjuncts.push_back(body);

	bit_slice_memo<node> slice_memo;
	using tbdd = tau_term_bdd<node>;
	std::vector<typename tbdd::ref> conjunct_bdds;
	conjunct_bdds.reserve(conjuncts.size());
	for (tref c : conjuncts) {
		auto cbdd = build_wff_bdd<node>(c, bit_ord, slice_memo);
		if (!cbdd) return result; // decline path: keep today's result
		conjunct_bdds.push_back(*cbdd);
	}
	std::vector<subtree_set<node>> supports(conjuncts.size());
	for (size_t k = 0; k < conjuncts.size(); ++k) {
		std::unordered_set<typename tbdd::ref> seen;
		collect_bdd_support<node>(conjunct_bdds[k], supports[k], seen);
	}

	const int_t no_bit = static_cast<int_t>(bit_ord.size()); // sorts last
	std::vector<std::tuple<int_t, int_t, size_t>> ranked;
	ranked.reserve(conjuncts.size());

	// Branch-aware conjunct grouping. Global rank-only order interleaves
	// independent aux-var chains bit-by-bit (bit_ord is bit-major across
	// ALL occurring variables), so a shared producer feeding two
	// downstream consumer chains keeps BOTH chains' carry/borrow bits
	// simultaneously live for most of the fold -- `running` then carries
	// their joint state, which is what grows geometrically. Fix: identify
	// which aux var each conjunct "defines" -- the aux bit with the
	// highest bit_ord rank in its support, sound because a recurrence
	// conjunct's output bit always outranks its inputs (bit-major: a
	// higher bit index always outranks any bit at a lower or equal
	// index) -- group conjuncts by that owner, and schedule whole groups
	// in producer-before-consumer (topological) order so one chain
	// retires before an unrelated one starts, instead of interleaving.
	// Within a group, and among ungrouped (no-aux) conjuncts, the
	// highest-rank-first order is kept unchanged.
	//
	// This only changes the SCHEDULE (a permutation of conjuncts);
	// last_use_bit/elim_at below are recomputed from whatever order
	// results, so soundness never depends on this order being a "good"
	// one -- only correctness-relevant property is that it is total,
	// which any permutation satisfies.
	std::vector<int_t> max_rank_of(conjuncts.size()), min_rank_of(conjuncts.size());
	std::vector<tref> owner_of(conjuncts.size(), nullptr);
	for (size_t k = 0; k < conjuncts.size(); ++k) {
		int_t max_rank = -1, min_rank = no_bit, aux_max_rank = -1;
		tref aux_owner = nullptr;
		for (tref b : supports[k]) {
			int_t r = bit_ord.at(b);
			max_rank = std::max(max_rank, r);
			min_rank = std::min(min_rank, r);
			if (aux_bits.contains(b) && r > aux_max_rank) {
				aux_max_rank = r;
				aux_owner = bit_owner.at(b);
			}
		}
		if (supports[k].empty()) max_rank = min_rank = no_bit;
		max_rank_of[k] = max_rank;
		min_rank_of[k] = min_rank;
		owner_of[k] = aux_owner; // nullptr: conjunct defines/uses no aux var
	}

	// Group conjuncts by owner; group_order_seen keeps first-seen order
	// for a deterministic DFS start order below.
	subtree_unordered_map<node, std::vector<size_t>> groups;
	std::vector<tref> group_order_seen;
	std::vector<size_t> ungrouped;
	for (size_t k = 0; k < conjuncts.size(); ++k) {
		if (tref o = owner_of[k]; o) {
			if (!groups.contains(o)) group_order_seen.push_back(o);
			groups[o].push_back(k);
		} else ungrouped.push_back(k);
	}

	// Producer -> consumer edges: group g depends on every other aux var
	// whose bits g's own conjuncts reference -- but only those that are
	// themselves a real group (own at least one conjunct as their
	// argmax-rank owner). An aux var can be referenced via bit_owner
	// without ever being any conjunct's argmax owner (e.g. its own
	// defining conjunct's argmax happens to be a different, more
	// downstream aux var's bit) -- such a var has no conjuncts of its
	// own to schedule as a block, so it is not a DFS/finish_order node;
	// skipping it here is what keeps groups.at(g) below total.
	subtree_unordered_map<node, trefs> deps;
	for (tref g : group_order_seen) {
		subtree_set<node> d;
		for (size_t k : groups.at(g))
			for (tref b : supports[k])
				if (aux_bits.contains(b))
					if (tref o = bit_owner.at(b); o != g && groups.contains(o))
						d.insert(o);
		deps.emplace(g, trefs(d.begin(), d.end()));
	}

	// Iterative DFS topological sort (dependencies finish before the
	// group that depends on them): a group is appended to finish_order
	// only after every group it depends on already is, which keeps each
	// chain's own conjuncts contiguous and orders whole chains
	// producer-before-consumer.
	std::vector<tref> finish_order;
	subtree_set<node> visited;
	for (tref start : group_order_seen) {
		if (visited.contains(start)) continue;
		std::vector<std::pair<tref, size_t>> stack;
		stack.emplace_back(start, 0);
		visited.insert(start);
		while (!stack.empty()) {
			auto& [g, idx] = stack.back();
			const trefs* dl = nullptr;
			if (auto it = deps.find(g); it != deps.end()) dl = &it->second;
			if (dl && idx < dl->size()) {
				tref d = (*dl)[idx];
				++idx;
				if (visited.insert(d).second) stack.emplace_back(d, 0);
			} else {
				finish_order.push_back(g);
				stack.pop_back();
			}
		}
	}

	auto by_rank = [&](size_t a, size_t b) {
		return std::tie(max_rank_of[a], min_rank_of[a])
			< std::tie(max_rank_of[b], min_rank_of[b]);
	};
	auto append_sorted = [&](std::vector<size_t> idxs) {
		std::stable_sort(idxs.begin(), idxs.end(), by_rank);
		for (size_t k : idxs)
			ranked.emplace_back(max_rank_of[k], min_rank_of[k], k);
	};
	for (tref g : finish_order) append_sorted(groups.at(g));
	append_sorted(ungrouped);

	// Last conjunct (in the sorted/fold order) whose support reaches each
	// aux bit -- that's when it becomes safe to existentially close that
	// bit. Only bits in aux_bits (aux-variable images) are ever tracked
	// here, so free-variable bits are never targeted for elimination.
	std::unordered_map<tref, size_t> last_use_bit;
	for (size_t j = 0; j < ranked.size(); ++j) {
		size_t k = std::get<2>(ranked[j]);
		for (tref b : supports[k])
			if (aux_bits.contains(b)) last_use_bit[b] = j;
	}
	std::vector<trefs> elim_at(ranked.size());
	for (tref b : aux_bits)
		if (auto it = last_use_bit.find(b); it != last_use_bit.end())
			elim_at[it->second].push_back(b);
		// Absent from every conjunct's support: nothing to eliminate, the
		// BDD never mentions this bit in the first place.

	// Defensive budget: 500k unique BDD nodes. TAU_CACHE is off in Debug
	// (project convention), so bdd_and/bdd_ex recursion has no memo to
	// fall back on if the live frontier turns out bigger than expected
	// for some formula shape early quantification doesn't help with, while
	// the size check itself (a bounded traversal) stays cheap below that
	// ceiling. Exceeding it aborts to the same fallback a decline uses --
	// the path must be structurally unable to hang, not just fast in the
	// common case.
	constexpr size_t NODE_BUDGET = 500'000;
	std::unordered_set<typename tbdd::ref> size_scratch;
	auto node_count_of = [&](typename tbdd::ref x) {
		size_scratch.clear();
		return bdd_node_count<node>(x, size_scratch);
	};
	// Cache of `running`'s own node count, refreshed only when it matters
	// rather than on every batch (per-bit scheduling reassigns `running`
	// every loop iteration, so a real bdd_node_count scan on every change
	// would itself dominate wall time):
	//  - periodically (every RESCAN_EVERY_BATCHES batches), so a
	//    systematic gap between the cheap additive estimate and
	//    bdd_and_many's actual (possibly non-additive, due to internal
	//    sharing/cancellation) result size cannot silently accumulate
	//    past the budget between true-ups;
	//  - as soon as the estimate gets within RESCAN_HEADROOM_FRACTION of
	//    the budget, so the expensive-but-exact check is available with
	//    margin before a true overrun, when it matters most.
	// Between true-ups, `running_nodes` is simply set to the cheap
	// additive estimate (already gated at <= NODE_BUDGET by the pre-fold
	// decline below), which keeps the pre-fold estimate for the next
	// batch reasonably accurate without paying for a real scan.
	constexpr size_t RESCAN_EVERY_BATCHES = 8;
	constexpr double RESCAN_HEADROOM_FRACTION = 0.5;
	size_t running_nodes = 0;
	size_t batches_since_rescan = 0;
	// Always-exact rescan: used as the backstop itself, and after every
	// aux-bit elimination (bdd_ex), whose effect on BDD size has no cheap
	// additive proxy the way bdd_and_many's operands do.
	auto rescan = [&](typename tbdd::ref x) {
		running_nodes = node_count_of(x);
		batches_since_rescan = 0;
		return running_nodes;
	};
	auto over_budget = [&](typename tbdd::ref x) {
		return rescan(x) > NODE_BUDGET;
	};
	// Amortized post-fold check: `estimate` is the same additive estimate
	// already computed pre-fold for this batch (see new_members_estimate
	// below), reused here as a cheap stand-in for `x`'s true node count.
	auto over_budget_after_fold = [&](typename tbdd::ref x, size_t estimate) {
		if (batches_since_rescan + 1 >= RESCAN_EVERY_BATCHES ||
			estimate >= size_t(NODE_BUDGET * RESCAN_HEADROOM_FRACTION))
			return rescan(x) > NODE_BUDGET;
		running_nodes = estimate;
		++batches_since_rescan;
		return false;
	};
	// Pre-emptive budget: `over_budget(running)` below only fires AFTER
	// bdd_and_many has already paid for the fold. Estimate the cost from
	// running's cached size plus this batch's NEW members only (each
	// counted independently -- cheap, bounded traversals, no aliasing
	// assumed) and decline before spending that time if it already looks
	// too big; the post-call check stays as the backstop for folds whose
	// RESULT balloons past what the operands alone predicted.
	auto new_members_estimate = [&](const typename tbdd::refs& new_members) {
		size_t total = running_nodes;
		for (auto& b : new_members) total += node_count_of(b);
		return total;
	};

	// Multiway batching: instead of folding conjuncts into `running` one
	// bdd_and at a time, gather every consecutive conjunct up to (and
	// including) the next one that triggers an elimination into a single
	// batch, and combine `running` with the whole batch via bdd_and_many
	// in one call -- bdd_and_many's multi-way Shannon split (smallest
	// variable across ALL operands at once, with subsumption elimination)
	// does far less redundant work than the same result built through
	// repeated pairwise bdd_and. Mirrors the same fix already applied to
	// zero_test's per-bit ANDing above. Batches may now be a single
	// conjunct (or a small handful) -- that is the point of per-bit
	// scheduling: the frontier between elimination points is meant to
	// stay short.
	//
	// A batch boundary must fall exactly at elim_at[j]: eliminating an aux
	// bit is only sound once the last conjunct reaching it (per the
	// last_use_bit computation above) is already folded into `running`,
	// so that conjunct must be the LAST member of the batch immediately
	// preceding the elimination -- the next batch starts right after it.
	typename tbdd::ref running = tbdd::T;
	size_t batch_start = 0;
	for (size_t j = 0; j < ranked.size(); ++j) {
		if (elim_at[j].empty() && j + 1 < ranked.size())
			continue; // extend the batch; not yet at a boundary or the end
		typename tbdd::refs batch;
		batch.reserve(j - batch_start + 2);
		batch.push_back(running);
		typename tbdd::refs new_members;
		new_members.reserve(j - batch_start + 1);
		for (size_t m = batch_start; m <= j; ++m) {
			auto c = conjunct_bdds[std::get<2>(ranked[m])];
			batch.push_back(c);
			new_members.push_back(c);
		}
		size_t estimate = new_members_estimate(new_members);
		if (estimate > NODE_BUDGET) return result;
		running = tbdd::bdd_and_many(std::move(batch), bit_ord);
		if (over_budget_after_fold(running, estimate)) return result;
		if (!elim_at[j].empty()) {
			running = tbdd::bdd_ex(running, elim_at[j], bit_ord);
			if (over_budget(running)) return result;
		}
		batch_start = j + 1;
	}
	std::unordered_map<typename tbdd::ref, tref> wff_memo;
	return bdd_to_wff<node>(running, wff_memo);
}

/**
 * @brief Performs predicate blasting for bitvector terms.
 *
 * Traverses the term, replacing supported bitvector operations with existentially
 * quantified variables and predicates.
 *
 * @tparam node Node type
 * @param term The term to blast
 * @param changes Map of subtree replacements
 * @param vars Vector to collect introduced variables
 * @return The blasted predicate, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // Called by every *_predicate function to resolve embedded arithmetic
 * // before the top-level comparison itself is handled; see @ref lt_predicate
 * // and @ref bvadd for the arithmetic this rewrites in practice.
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 8 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static std::pair<tref /* predicate */, tref /* transformed */> atomic_blasting(tref term, trefs& vars, subtree_map<node, tref>& changes) {
	using tau = tree<node>;

	tref predicate = nullptr;
	bool error = false;
	auto type_id = tau::get(term).get_ba_type();

	// The result variable of an operation must carry that operation's own BA
	// type, not the atomic's. Under a width-changing cast the two differ:
	// for `(bv[4]) (a + b) = c` with `a, b : bv[8]` the atomic is bv[4], so
	// the addend's result variable would be 4 bits wide while `bvadd`
	// derives its bit count from the 8-bit augend, and the constraint mixes
	// two widths (cvc5 raises "Subexpressions must have the same type").
	// The atomic's type stays as the fallback for an untyped operation node.
	auto result_type_of = [type_id](tref t) {
		size_t t_id = tau::get(t).get_ba_type();
		return t_id ? t_id : type_id;
	};

	// Operands may have been replaced by fresh variables already (post-order
	// traversal blasts inner operations first), so resolve them through the
	// changes map, defaulting to the original subtree.
	//
	// `changes` is keyed by the *bare* operation node, while every probe
	// passes the parent's child, which the grammar wraps in a `bf`
	// (`bf_add => bf '+' bf`). The key is therefore trimmed on lookup, and
	// the fresh variable re-wrapped on a hit, so the operand shape a caller
	// gets back is the same either way. Probing with the wrapped child never
	// matched, which left the outer constraint built over the raw inner
	// arithmetic term instead of the inner fresh variable.
	auto lookup = [&changes](tref c) -> tref {
		auto it = changes.find(tau::trim(c));
		return it != changes.end() ? tau::get(tau::bf, it->second) : c;
	};

	// Conjoin a new constraint into the accumulated predicate; a nullptr
	// constraint (failed blasting) is an error.
	auto conjoin = [&](tref current) {
		if (!current) { error = true; return; }
		predicate = predicate
			? build_wff_and<node>(predicate, current)
			: current;
	};

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();

		switch (nt) {
			case tau::bf_add: case tau::bf_sub: {
				auto result = build_variable<node>(result_type_of(t));
				auto bf_result = tau::get(tau::bf, result);
				auto left = lookup(tau::get(t).child(0));
				auto right = lookup(tau::get(t).child(1));
				vars.push_back(result);
				changes[t] = result;
				conjoin((nt == tau::bf_add)
					? bvadd<node>(left, right, bf_result, vars)
					: bvsub<node>(left, right, bf_result, vars));
				break;
			}
			case tau::bf_shl: case tau::bf_shr: {
				auto [shiftand_raw, count] = get_arguments<node>(t);
				if (!count) { error = true; break; }
				auto shiftand = lookup(shiftand_raw);
				auto shifted = tau::build_variable(result_type_of(t));
				auto bf_shifted = tau::get(tau::bf, shifted);
				vars.push_back(shifted);
				changes[t] = shifted;
				conjoin((nt == tau::bf_shl)
					? bvshl<node>(shiftand, count, bf_shifted)
					: bvshr<node>(shiftand, count, bf_shifted));
				break;
			}
			case tau::bf_mul: {
				auto [factor_raw, constant] = get_bvmul_arguments<node>(t);
				if (!constant) { error = true; break; }
				auto factor = lookup(factor_raw);
				auto product = tau::build_variable(result_type_of(t));
				auto bf_product = tau::get(tau::bf, product);
				vars.push_back(product);
				changes[t] = product;
				conjoin(bvmul<node>(factor, constant, bf_product, vars));
				break;
			}
			case tau::bf_div: case tau::bf_mod: {
				auto [dividend_raw, divisor] = get_arguments<node>(t);
				if (!divisor) { error = true; break; }
				auto dividend = lookup(dividend_raw);
				auto result = tau::build_variable(result_type_of(t));
				auto bf_result = tau::get(tau::bf, result);
				vars.push_back(result);
				changes[t] = result;
				conjoin((nt == tau::bf_mod)
					? bvmod<node>(dividend, divisor, bf_result, vars)
					: bvdiv<node>(dividend, divisor, bf_result, vars));
				break;
			}
			case tau::bf_cast: {
				auto child = tau::get(t).child(0);
				auto src = lookup(child);
				auto target_type_id = tau::get(t).get_ba_type();
				auto result = tau::build_variable(target_type_id);
				auto bf_result = tau::get(tau::bf, result);
				auto src_width = get_bv_type_bitwidth<node>(src);
				auto target_width = get_bv_type_bitwidth<node>(result);
				// Same-size cast: just substitute with the source.
				// src is a bf node while t (bf_cast) sits under a bf
				// wrapper, so strip the bf level to avoid bf { bf {..} }.
				if (src_width == target_width) {
					changes[t] = tau::trim(src);
					break;
				}
				// Different sizes: introduce variable and predicate
				vars.push_back(result);
				changes[t] = result;
				conjoin(bvcast<node>(src, bf_result));
				break;
			}
			// Nand, nor and xnor are treated in the hooks so we never get those
			// cases at this point.
			default: {

				break;
			}
		}
		// The visitor contract is: return true to continue the traversal,
		// false to abort it (we only abort on error).
		return !error;
	};

	post_order<node>(term).search_unique(f);
	// If we have an unsupported operation, we return nullptr to indicate failure
	if (error) return { nullptr, nullptr };
	// We reconstruct the original bf and wrap it
	auto modified = rewriter::replace<node>(term, changes);
	return { predicate, modified };
}

/**
 * @internal
 * @brief Blasts an atomic comparison whose top-level relation itself needs
 * no per-bit decomposition (equality/inequality), only its arithmetic
 * sub-terms do.
 *
 * Runs `atomic_blasting` to rewrite any embedded arithmetic (e.g. `x+1`)
 * into a fresh variable plus a conjoined constraint, then returns the
 * (possibly unchanged) comparison conjoined with that constraint,
 * existentially quantifying the fresh auxiliary variables. Shared plumbing
 * for `eq_predicate` and `neq_predicate`.
 * @tparam node Tree node type.
 * @param atomic The atomic `=`/`!=` comparison to blast.
 * @return The blasted predicate, or `nullptr` on error.
 *
 * @par Example
 * @code{.cpp}
 * // "x + { 1 }:bv[4] = { 5 }:bv[4]": the embedded addition is blasted into
 * // a fresh variable constrained by bvadd, conjoined with the equality.
 * tref fm = get_nso_rr(
 *     "x + { 1 }:bv[4] = { 5 }:bv[4].").value().main->get();
 * tref atomic = tau::trim(tau::get(fm)[0].get());
 * tref res = keep_comparison_predicate<node_t>(atomic);
 * CHECK( res != nullptr );
 * @endcode
 * @endinternal
 */
template<NodeType node>
static tref keep_comparison_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;
	auto [predicate, blasted] = atomic_blasting<node>(atomic, vars, changes);
	if (!blasted) return nullptr;

	// If the atomic contains no blastable arithmetic, predicate is null and
	// the (unchanged) atomic itself is the result.
	auto wff_blasted = tau::get(tau::wff, blasted);
	return quantify_aux_vars<node>(vars, predicate
		? tau::build_wff_and(predicate, wff_blasted)
		: wff_blasted);
}

/**
 * @brief Blasts an equality predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic equality predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "=" atoms by wff_predicate_blasting (see bf_eq case);
 * // exercised e.g. by "x + { 5 }:bv[4] = { 8 }:bv[4]" (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:70-72).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 8 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref eq_predicate(tref atomic) {
	return keep_comparison_predicate<node>(atomic);
}

/**
 * @brief Blasts an inequality predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic inequality predicate
 * @return The resulting predicate term, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!=" atoms; x != x is never satisfiable (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:546-547).
 * tref fm = get_nso_rr("ex x x:bv[4] != x:bv[4].").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_F() );
 * @endcode
 */
template<NodeType node>
static tref neq_predicate(tref atomic) {
	return keep_comparison_predicate<node>(atomic);
}

/**
 * @brief Blasts a less-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic less-than predicate
 * @return The resulting predicate term, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "<" atoms, delegating to bvlt after atomic_blasting
 * // resolves any embedded arithmetic (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:316-317).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 2 }:bv[2] && x < { 3 }:bv[2]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref lt_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;

	auto [predicate, blasted] = atomic_blasting<node>(atomic, vars, changes);
	if (!blasted) return nullptr;

	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto applied = bvlt<node>(left, right);
	if (!applied) return nullptr;

	return quantify_aux_vars<node>(vars, predicate
		? tau::build_wff_and(predicate, applied)
		: applied);
}

/**
 * @brief Blasts a greater-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic greater-than predicate
 * @return The resulting predicate term, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for ">" atoms; x > x is never satisfiable (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:362-363).
 * tref fm = get_nso_rr("ex x x:bv[4] > x:bv[4].").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_F() );
 * @endcode
 */
template<NodeType node>
static tref gt_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;

	auto [predicate, blasted] = atomic_blasting<node>(atomic, vars, changes);
	if (!blasted) return nullptr;

	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto applied = bvgt<node>(left, right);
	if (!applied) return nullptr;

	return quantify_aux_vars<node>(vars, predicate
		? tau::build_wff_and(predicate, applied)
		: applied);
}

/**
 * @brief Blasts a less-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic less-than-or-equal predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "<=" atoms: 2 <= 3 for 2-bit bitvectors (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:415-417).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 2 }:bv[2] && x <= { 3 }:bv[2]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref lteq_predicate(tref atomic) {
	using tau = tree<node>;

	auto inner = gt_predicate<node>(atomic);
	return inner ? tau::build_wff_neg(inner) : nullptr;
}

/**
 * @brief Blasts a greater-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic greater-than-or-equal predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for ">=" atoms: all x, x >= x (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:389-391).
 * tref fm = get_nso_rr("all x x:bv[4] >= x:bv[4].").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref gteq_predicate(tref atomic) {
	using tau = tree<node>;

	auto inner = lt_predicate<node>(atomic);
	return inner ? tau::build_wff_neg(inner) : nullptr;
}

/**
 * @brief Blasts a not-less-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-less-than predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!<" atoms: 2 !< 2 is T (equal case) (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:850-851).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 2 }:bv[4] && x !< { 2 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref nlt_predicate(tref atomic) {
	using tau = tree<node>;

	auto inner = lt_predicate<node>(atomic);
	return inner ? tau::build_wff_neg(inner) : nullptr;
}

/**
 * @brief Blasts a not-greater-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-greater-than predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!>" atoms: 2 !> 2 is T (equal case) (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp,
 * // TEST_SUITE("bvngt"), "bvngt: 2 !> 2 is T (equal case)").
 * tref fm = get_nso_rr(
 *     "ex x (x = { 2 }:bv[4] && x !> { 2 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref ngt_predicate(tref atomic) {
	using tau = tree<node>;

	auto inner = gt_predicate<node>(atomic);
	return inner ? tau::build_wff_neg(inner) : nullptr;
}

/**
 * @brief Blasts a not-less-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-less-than-or-equal predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!<=" atoms: 3 !<= 1 is T (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:433-434).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[2] && x !<= { 1 }:bv[2]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref nlteq_predicate(tref atomic) {
	auto applied = gt_predicate<node>(atomic);
	return applied;
}

/**
 * @brief Blasts a not-greater-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-greater-than-or-equal predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!>=" atoms: 0 !>= 1 is T (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp,
 * // TEST_SUITE("bvngteq"), "bvngteq: 0 !>= 1 is T").
 * tref fm = get_nso_rr(
 *     "ex x (x = { 0 }:bv[4] && x !>= { 1 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref ngteq_predicate(tref atomic) {
	auto applied = lt_predicate<node>(atomic);
	return applied;
}

/**
 * @brief Recursively blasts all bitvector formulas in a formula.
 *
 * Traverses the formula, replacing atomic bitvector predicates with their blasted forms.
 *
 * @tparam node Node type
 * @param term The formula to blast
 * @return The formula with predicates blasted, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // This is what bv_predicate_blasting (the public entry point) calls
 * // internally; see @ref bv_predicate_blasting for a worked example.
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 8 }:bv[4]).").value().main->get();
 * tref blasted = wff_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
/**
 * @brief Does @p atomic contain a variable outside the bv family, or a
 * bv-typed variable with no explicit bitwidth?
 *
 * Blasting only rewrites arithmetic/comparisons over bv-typed operands; a
 * variable left outside that family (e.g. one type inference never gave a
 * bv type, left at ba_type 0) is nothing blasting can resolve, and the
 * blasting machinery assumes every variable it walks is bv-typed with a
 * known bitwidth. Used to decline blasting such an atom cleanly (no
 * change) instead.
 *
 * @tparam node Node type
 * @param atomic The atomic comparison to check
 * @return true if a disqualifying variable is present
 */
template<NodeType node>
static bool has_non_bv_operand(tref atomic) {
	using tau = tree<node>;
	bool bad = false;
	auto check = [&](tref n) {
		if (!is<node, tau::variable>(n)) return true;
		size_t t = tau::get(n).get_ba_type();
		if (!is_bv_type_family<node>(t)
			|| get_bv_type_bitwidth<node>(n) == 0)
			return bad = true, false;
		return true;
	};
	pre_order<node>(atomic).search_unique(check);
	return bad;
}

template<NodeType node>
static tref wff_predicate_blasting(tref term) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	bool error = false;

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();

		auto rebuild_default = [&]() {
			// just replace the children if needed, the current node will be
			// reconstructed in the post-order traversal
			trefs ch;
			for (tref c : tau::get(t).children()) {
				if (changes.find(c) != changes.end())
					ch.push_back(changes[c]);
				else ch.push_back(c);
			}

			if (auto new_t = tau::get(tau::get(t).value, ch.data(), ch.size()); new_t != t)
				changes[t] = new_t;
		};

		auto blast = [&](tref atomic, const auto& blaster) -> tref {
				// Decline cleanly (no change to this atom) rather than
				// hand a mixed-type atom to machinery that assumes every
				// operand is bv-typed -- see has_non_bv_operand.
				if (has_non_bv_operand<node>(atomic)) return t;
				auto blasted = blaster(atomic);
				if (!blasted) return error = true, t;
				// The atomic being replaced sits under a wff node and
				// the blasters return wff-level trees, so strip the
				// wff wrapper to avoid wff { wff { ... } } nestings
				// that defeat hook-based simplification.
				changes[t] = tau::trim(blasted);
				return blasted;
		};

		// The ordering relations (<, >, <=, >=, their negations, and the
		// ternary interval) are bitvector-specific bit-recurrences here:
		// they unconditionally treat their operands as bitvectors (see
		// bvlt/bvgt/... in bv_predicate_blasting_comparisons.tmpl.h).
		// Other BA types (qlt, sbf, tau, hsb, nlang, ...) reuse these
		// same node kinds for their own orderings, already resolved by
		// their own hooks (e.g. qlt_singleton_cmp in hooks_tau.tmpl.h).
		// Blasting a non-bv atom here misreads its operand's bitwidth as
		// 0 (get_bv_type_bitwidth logs an error and returns 0), which
		// then underflows bitwidth-1 into a huge index and corrupts
		// downstream state. Only dispatch to the bv blasters when at
		// least one operand actually carries a bv BA-type.
		auto is_bv_operands = [&]() {
			// HE-10: the ordering blasters read the bitwidth from the
			// LEFT operand, so the left one must actually carry the
			// bv type -- an OR-gate admitted left-untyped/right-bv
			// atoms whose width read 0 and underflowed bitwidth-1
			// (the exact corruption the comment above describes).
			// Declining such a half-typed atom to rebuild_default is
			// conservative; type unification stamps both sides on
			// every real path.
			tref l = tau::get(t).child(0);
			return is_bv_type_family<node>(tau::get(l).get_ba_type());
		};

		switch (nt) {
			case tau::bf_eq: { blast(t, eq_predicate<node>); break; }
			case tau::bf_neq: { blast(t, neq_predicate<node>); break; }
			case tau::bf_lt: {
				if (is_bv_operands()) blast(t, lt_predicate<node>);
				else rebuild_default();
				break;
			}
			case tau::bf_gt: {
				if (is_bv_operands()) blast(t, gt_predicate<node>);
				else rebuild_default();
				break;
			}
			case tau::bf_lteq: {
				if (is_bv_operands()) blast(t, lteq_predicate<node>);
				else rebuild_default();
				break;
			}
			case tau::bf_gteq: {
				if (is_bv_operands()) blast(t, gteq_predicate<node>);
				else rebuild_default();
				break;
			}
			case tau::bf_nlt: {
				if (is_bv_operands()) blast(t, nlt_predicate<node>);
				else rebuild_default();
				break;
			}
			case tau::bf_ngt: {
				if (is_bv_operands()) blast(t, ngt_predicate<node>);
				else rebuild_default();
				break;
			}
			case tau::bf_nlteq: {
				if (is_bv_operands()) blast(t, nlteq_predicate<node>);
				else rebuild_default();
				break;
			}
			case tau::bf_ngteq: {
				if (is_bv_operands()) blast(t, ngteq_predicate<node>);
				else rebuild_default();
				break;
			}
			case tau::bf_interval: {
				tref lo  = tau::get(t)[0].get();
				tref mid = tau::get(t)[1].get();
				tref hi  = tau::get(t)[2].get();
				if (!is_bv_type_family<node>(tau::get(lo).get_ba_type())
					&& !is_bv_type_family<node>(tau::get(mid).get_ba_type())
					&& !is_bv_type_family<node>(tau::get(hi).get_ba_type())) {
					rebuild_default();
					break;
				}
				// a <= b <= c is equivalent to (a <= b) && (b <= c);
				// blast each comparison separately and conjoin the results.
				tref lo_le_mid = tau::get(tau::bf_lteq, lo, mid);
				tref mid_le_hi = tau::get(tau::bf_lteq, mid, hi);
				tref left  = lteq_predicate<node>(lo_le_mid);
				tref right = lteq_predicate<node>(mid_le_hi);
				if (!left || !right) { error = true; break; }
				changes[t] = tau::trim(tau::build_wff_and(left, right));
				break;
			}
			default: {
				rebuild_default();
				break;
			}
		}
		// The visitor contract is: return true to continue the traversal,
		// false to abort it (we only abort on error).
		return !error;
	};

	post_order<node>(term).search_unique(f);

	if (error) {
		DBG(LOG_DEBUG << "Failed to compute predicate blasting in: " << LOG_FM(term);)
		return nullptr;
	}

	// quantify_aux_vars already anti-prenexes each blasted atomic's own
	// freshly-introduced auxiliary quantifiers (scoped locally, so a
	// still-unresolved genuinely bv-typed quantifier elsewhere in `term`
	// is left untouched); nothing further to do here.
	return changes.find(term) != changes.end() ? changes[term] : term;
}

/**
 * @brief Entry point for predicate blasting on bitvector formulas.
 *
 * Alias for wff_predicate_blasting.
 *
 * @tparam node Node type
 * @param term The formula to blast
 * @return The formula with predicates blasted, or nullptr on error
 */
template<NodeType node>
inline tref bv_predicate_blasting(tref term) {
	auto ret = wff_predicate_blasting<node>(term);
	return ret;
}

} // namespace idni::tau_lang