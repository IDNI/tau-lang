// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "tau_tree.h"
#include "boolean_algebras/ba_pack_traits.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// Tau tree implementation declarations
// -----------------------------------------------------------------------------

struct rr_sig;
template <NodeType node> struct ba_types;
template <NodeType node> struct ba_constants;
template <NodeType node> struct get_hook;

// -----------------------------------------------------------------------------

/**
 * @brief Extract the signature of the recurrence-relation reference @p n:
 * its name interned via `rr_dict`, offset arity and argument arity.
 * @param n A `ref` node or a `bf_ref`/`wff_ref` wrapping one.
 */
template <NodeType node>
rr_sig get_rr_sig(tref n);

/**
 * @brief Collect the `rec_relation` definitions under @p r as rewriter
 * rules, one (head, body) pair per definition in tree order, with each
 * body's I/O variables classified against @p ctx (see resolve_io_vars,
 * tau_tree_extractors.tmpl.h); heads are kept verbatim. Accepts a single
 * `rec_relation` or a `start`/`spec` tree whose definitions section is
 * scanned; a null @p r yields an empty rule set.
 */
template <NodeType node>
rewriter::rules get_rec_relations(io_context<node>& ctx, tref r);
/** @brief Overload using the global `definitions` singleton's I/O context. */
template <NodeType node>
rewriter::rules get_rec_relations(tref r);

/**
 * @brief Split parsed input @p ref into recurrence rules plus a main
 * formula (an `rr`), classifying I/O variables in the main and the rule
 * bodies against @p ctx. A bare `bf`/`ref` becomes a rule-free main taken
 * verbatim; a single `rec_relation` yields rules with a null main.
 * @return a failed report when @p ref is null, no main formula is found,
 * an I/O variable remains unclassified after resolve_io_vars (neither a
 * declared stream nor named like one), or validate_rr_case_types or
 * validate_rr_call_types rejects the rules.
 */
template <NodeType node>
result<rr<node>> get_nso_rr(io_context<node>& ctx, tref ref);
/** @brief Overload using the global `definitions` singleton's I/O context. */
template <NodeType node>
result<rr<node>> get_nso_rr(tref ref);

/** @brief Declared for tau_spec; documented in satisfiability.h. */
template <NodeType node>
result<tref> pin_written_warm_ups(tref fm);

/**
 * @brief Append to @p leaves the maximal subtrees of @p n that are not
 * themselves headed by @p branch, flattening the (arbitrarily deep)
 * @p branch spine left to right. A subtree occurring several times is
 * appended once per occurrence; a null @p n appends nothing. Runs with
 * time and memory linear in the output size (see the note at the
 * definition in tau_tree_extractors.tmpl.h).
 */
template <NodeType node>
void get_leaves(tref n, typename node::type branch, trefs& leaves);
/** @brief As above, returning the leaves as a fresh vector. */
template <NodeType node>
trefs get_leaves(tref n, typename node::type branch);

/**
 * @brief Return the disjuncts of formula @p n: the leaves of its `wff_or`
 * spine, i.e. the clauses when @p n is in DNF. A disjunction-free @p n
 * comes back as the single element.
 */
template <NodeType node>
trefs get_dnf_wff_clauses(tref n);
/**
 * @brief Return the conjuncts of formula @p n: the leaves of its
 * `wff_and` spine, i.e. the clauses when @p n is in CNF.
 */
template <NodeType node>
trefs get_cnf_wff_clauses(tref n);
/**
 * @brief Return the disjuncts of term @p n: the leaves of its `bf_or`
 * spine, i.e. the clauses when @p n is in DNF.
 */
template <NodeType node>
trefs get_dnf_bf_clauses(tref n);
/**
 * @brief Return the conjuncts of term @p n: the leaves of its `bf_and`
 * spine, i.e. the clauses when @p n is in CNF.
 */
template <NodeType node>
trefs get_cnf_bf_clauses(tref n);

/**
 * @brief Provides a range over the paths of a term or formula. If the formula has
 * temporal quantifiers, a path is the conjunction of temporally quantified formulas.
 * In other words, disjunctions underneath a temporal quantifier are not taken
 * into account.
 *
 * Preconditions and contracts:
 * - For terms, `bf_xor` is treated as an or-fork: an xor's operands are
 *   enumerated as if they were disjuncts, and "removing" a path keeps only
 *   the other operand. This is sound only for the DNF-ish normal forms the
 *   consumers (solver, splitter, satisfiability, normalizer) feed it -- do
 *   not hand it arbitrary formulas.
 * - `apply(transform, callback)` invokes `callback(nullptr)` once on its
 *   first iteration before any path result exists.
 * @tparam node Tree node class
 */
template <NodeType node>
struct expression_paths {
	using tau = tree<node>;
	/// Wrap @p expr, the term or formula whose paths are enumerated.
	explicit expression_paths(tref expr) : _expr(expr) {}
	/// Forward iterator materializing one path per dereference, steered by
	/// a stack of left/right decisions, one per disjunction fork met so far.
	struct iterator {
		using iterator_category = std::forward_iterator_tag;
		using value_type = tref;
		using difference_type = std::ptrdiff_t;
		using pointer = tref*;
		using reference = tref&;

		/// Iterator positioned on the first path of @p expr; a null
		/// @p expr is the end iterator.
		explicit iterator(tref expr) : _expr(expr) {}
		/// Get path according to decisions
		value_type operator*();
		/// Adjust decisions to point to next path.
		/// WARNING: Only use after calling operator*() at least once on
		/// current state
		iterator& operator++();
		/// Apply the function f to current path and return the result
		/// while erasing the path from _expr; nullptr, with nothing
		/// erased, when f leaves the path unchanged or the expression is
		/// null.
		/// NOTE that this changes the result of operator*()
		tref apply(const auto& f);
		/// Restore the expression saved by the last apply().
		void undo_apply();
		/// The expression as it stands after the applies so far.
		tref get_expr() const { return _expr; }
		/// Equal when both wrap equal expressions and their decisions
		/// agree, a missing trailing decision counting as left.
		bool operator==(const iterator& other) const;
		/// Negation of operator==.
		bool operator!=(const iterator& other) const;
	private:
		// True is left, false is right
		std::vector<bool> decisions;
		bool keep_path = false;
		tref _expr;
		tref _prev_expr;
	};
	/// Iterator on the first path of the wrapped expression.
	iterator begin() const;
	/// The end iterator (a null expression).
	iterator end() const;
	/// Apply path_transform on each path while replacing the path with
	/// the result in the expression. Note that this affects Boole normal
	/// form structure.
	tref apply(const auto& path_transform);
	/// Apply path_transform on each path while replacing the path with
	/// the result in the expression. Note that this affects Boole normal
	/// form structure.
	/// If callback returns false, the current result is returned
	tref apply(const auto& path_transform, const auto& callback);
	/// Apply path_transform on the first pre-order path. If callback returns
	/// false on the result, leave the expression unchanged and try the next
	/// path until callback returns true or all paths are visited.
	/// Note that this affects Boole normal form structure, if callback
	/// returns true on any path
	tref apply_only_if(const auto& path_transform, const auto& callback);
private:
	tref _expr;
};

/** @brief Return the BA type id stored in @p n's node value. */
template <NodeType node>
size_t get_ba_type(tref n);

/**
 * @brief Descend from @p var to its naming leaf, unwrapping `bf`,
 * `variable` and `io_var` layers until a `var_name` or `uconst_name`
 * node is found; a `ba_constant` or `var_name` argument is returned
 * itself.
 * @return The naming node, or nullptr when none exists.
 */
template <NodeType node>
tref get_var_name_node(tref var);
/**
 * @brief Return the name string of @p var's naming leaf (see
 * get_var_name_node), or a reference to a static empty string when
 * there is none.
 */
template <NodeType node>
const std::string& get_var_name(tref var);
/**
 * @brief Return the dict string id of @p var's naming leaf, or 0 when
 * there is none.
 */
template <NodeType node>
size_t get_var_name_sid(tref var);

// The io_var helpers below expect the enclosing `variable` node whose
// first child is an `io_var` carrying an offset: x[3] addresses an
// absolute time point ("initial position"), x[t] the current step and
// x[t-N] a lookback of N steps ("shift").
/** @brief True iff @p io_var addresses an absolute (integer) time point. */
template <NodeType node>
bool is_io_initial(tref io_var);
/** @brief True iff @p io_var's offset is a shift of the form t-N. */
template <NodeType node>
bool is_io_shift(tref io_var);
/**
 * @brief Return the absolute time point of an initial-position variable.
 * Only valid when is_io_initial holds.
 */
template <NodeType node>
int_t get_io_time_point(tref io_var);
/**
 * @brief Return the lookback N of a shifted offset t-N. Only valid when
 * is_io_shift holds.
 */
template <NodeType node>
int_t get_io_shift(tref io_var);
/**
 * @brief Read a node's inline payload as `int_t`.
 * The shift, time-point and capture-name payloads it reads are small
 * non-negative values, so one past `int_t`'s range is a caller bug.
 */
template <NodeType node>
int_t get_payload_int(const tree<node>& n);
/**
 * @brief Return the lookback N for an offset of the form t-N, and 0 for
 * any other offset form (current step or absolute time point).
 */
template <NodeType node>
int_t get_io_var_shift(tref io_var);
/**
 * @brief Return the largest lookback (get_io_var_shift) over @p io_vars,
 * 0 when none is shifted. With @p ignore_temps, streams whose name
 * begins with '_' (interpreter temporaries) are skipped.
 */
template <NodeType node>
int_t get_max_shift(const trefs& io_vars, bool ignore_temps = false);
/**
 * @brief Return the largest absolute time point among the
 * initial-position variables of @p io_vars, or -1 when there is none.
 */
template <NodeType node>
int_t get_max_initial(const trefs& io_vars);

/**
 * @brief Return the free variables and captures of @p n, deduplicated
 * and sorted by `subtree_less`, excluding bound occurrences and the
 * offset variables inside io_vars; entries are stored without right
 * siblings. The result lives in a per-tree cache, so the reference
 * stays valid until that cache is cleared. A null @p n, or one that is
 * neither `bf` nor `wff`, yields a static empty vector.
 */
template <NodeType node>
const trefs& get_free_vars(tref n);

/**
 * @brief Partition @p fms into the connected groups of formulas linked by
 * a shared free variable of @p vars, preserving the order of @p fms.
 */
template <NodeType node>
std::vector<trefs> group_by_shared_vars(const trefs& fms, const trefs& vars);

/**
 * @brief True if @p n contains an io_var (any offset form, constant
 * positions included) or, lacking one, a `constraint`.
 */
template <NodeType node>
bool has_temp_var(tref n);

/**
 * @brief Report (as a warning in the result) a Tau-formula BA constant in
 * @p fm that is not closed, i.e. still has free variables. A constant not
 * converted yet (BA constant id 0) is skipped.
 * @return true when an open constant is found; the error of `is_closed`
 * on failure.
 */
template <NodeType node>
result<bool> has_open_tau_fm_in_constant(tref fm);

/**
 * @brief Whether a temporal quantifier nested inside another is invalid
 * in @p fm: always false, since full LTL lets temporal quantifiers nest
 * freely.
 */
template <NodeType node>
bool invalid_nesting_of_temp_quants(tref fm);

/**
 * @brief True when @p fm has a temporal quantifier and some part of it,
 * past the Boolean connectives, is not in the scope of one; the report
 * (info) names that part.
 */
template <NodeType node>
result<bool> missing_temp_quants(tref fm);

/**
 * @brief Report a non-temporal quantifier in @p fm whose bound variable
 * occurs free inside a temporal quantifier in its scope: a plain binding
 * may not reach across a temporal quantifier. The answer is true when the
 * formula has the problem; the report names the variable and the quantifier.
 */
template <NodeType node>
result<bool> invalid_nesting_of_quants(tref fm);

/**
 * @brief Report a recurrence-relation reference in @p fm carrying a
 * negative integer offset.
 * @return true when one is found, with an info naming the reference.
 */
template <NodeType node>
result<bool> has_negative_offset(tref fm);

/**
 * @brief Aggregate front-end validity gate: true when any of
 * invalid_nesting_of_quants, has_open_tau_fm_in_constant,
 * invalid_nesting_of_temp_quants, missing_temp_quants,
 * has_negative_offset or has_missplaced_fallback rejects @p fm.
 * invalid_nesting_of_quants and missing_temp_quants name the offending
 * part; every other check carries its own error message. Fails when
 * has_open_tau_fm_in_constant or has_negative_offset fails.
 */
template <NodeType node>
result<bool> has_semantic_error(tref fm);

/**
 * @brief Rewrite `G(A && G(B))` to `G(A) && G(B)`, recursively, so a
 * parse of `G(A) && G(B)` reads back as written. Returns @p fm unchanged
 * when there is nothing to rewrite; a null @p fm is returned as is.
 */
template <NodeType node>
tref unnest_nested_always(tref fm);

} // namespace idni::tau_lang

#include "dict.h"      // string pool accessed with dict() functions
#include "rr.h"        // recurrence relations structure
#include "ba_types.h"  // ba types dictionary

#include "tau_tree_builders.h"

#include "ref_types_inference.h"
#include "ba_types_inference.h"

#include "io_context.h"

// ADT type registry declarations. adt_types.tmpl.h (which needs the
// traverser and select_all, not yet defined here) is included further below,
// next to tau_tree_from_parser.tmpl.h -- see adt_types.h's file header.
#include "adt/adt_types.h"
// ADT flattener declarations; same split as adt_types.h/.tmpl.h, for the
// same reason -- see adt_flatten.h's file header.
#include "adt/adt_flatten.h"

#include "tau_tree_node.tmpl.h"
#include "tau_tree_traverser.tmpl.h"
#include "tau_tree_printers.tmpl.h"
#include "tau_tree_queries.tmpl.h"

namespace idni::tau_lang {
	// Forward declaration required by tau_tree_extractors.tmpl.h to handle BDD_ID
	// nodes in get_free_vars (full definition provided by tau_bdd.h).
	template <NodeType node> struct tau_term_bdd_handle;
} // namespace idni::tau_lang

#include "tau_tree_extractors.tmpl.h"  // TODO rename this file to proper name?
#include "tau_tree_from_parser.tmpl.h"

// Must come after tau_tree_from_parser.tmpl.h: adt_types.tmpl.h uses the
// traverser (`tt`) and `select_all`, defined earlier in this same block
// (tau_tree_traverser.tmpl.h, tau_tree_queries.tmpl.h). See adt_types.h's
// file header for why this is split from its own include.
#include "adt/adt_types.tmpl.h"
// adt_flatten.tmpl.h uses adt_registry (just included above), the traverser,
// is<node,...>, and tree<node>::build_* (declared in tau_tree.h, included at
// the top of this file, with tau_tree_builders.h/.tmpl.h already pulled in
// via tau_tree_builders.h above) -- see adt_flatten.h's file header.
#include "adt/adt_flatten.tmpl.h"

#include "hooks.h"

#include "tau_spec.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "tau_tree"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// Tau tree templates implementation

//------------------------------------------------------------------------------
// handles

template <NodeType node>
tref tree<node>::get() const { return base_t::get(); }

template <NodeType node>
const tree<node>& tree<node>::get(const tref id) {
	DBG(assert(id != nullptr);)
	return (const tree&) base_t::get(id);
}

template <NodeType node>
const tree<node>& tree<node>::get(const htref& h) {
	return (const tree&) base_t::get(h);
}

template <NodeType node>
htref tree<node>::geth(tref h) {
	DBG(assert(h != nullptr);)
	return base_t::geth(h);
}

template <NodeType node>
size_t tree<node>::m_size() {
	return bintree<node>::M().size();
}

template <NodeType node>
void tree<node>::clear_caches() {
	std::unique_lock lock(bintree<node>::mutex());
	std::unordered_set<tref> interned;
	for (const auto& [n, _] : bintree<node>::M()) interned.insert(n.get());
	for (const auto& cb : bintree<node>::gc_callbacks) cb({});
	for (const auto& cb : bintree<node>::gc_callbacks) cb(interned);
}

template <NodeType node>
htref tree<node>::geth(const tree& n) {
	return geth(n.get());
}

//------------------------------------------------------------------------------
// creation with tref childs

template <NodeType node>
tref tree<node>::get_raw(const node& v, const tref* ch, size_t len, tref r) {
	// LOG_DEBUG << "get_raw: " << v << " " << len << " r: " << r << "\n";
	return base_t::get_raw(v, ch, len, r);
}

template <NodeType node>
tref tree<node>::get(const node& v) {
	return get(v, (const tref*) nullptr, 0, nullptr);
}

template <NodeType node>
tref tree<node>::get(const node& v, tref ch) {
	return get(v, &ch, 1);
}

template <NodeType node>
tref tree<node>::get(const node& v, tref ch1, tref ch2) {
	return get(v, { ch1, ch2 });
}

template <NodeType node>
tref tree<node>::get(const node& v, const tref* ch, size_t len, tref r) {
	auto get_type = [](const node& n, const tref* ch, size_t len) -> size_t {
		if (n.ba_type != 0) return n.ba_type;
		// Short-circuit on first non-zero child type
		for (size_t i = 0; i < len; ++i) {
			size_t child_type = tau::get(ch[i]).get_ba_type();
			if (child_type != 0) return child_type;
		}
		return n.ba_type;
	};
	if (!base_t::use_hooks) return get_raw(v, ch, len, r);
	// get with hooks: set hook first if not hooked. Construct the
	// (stateless) get_hook inside the lambda -- the stored std::function
	// outlives this call, so capturing a local by reference would dangle.
	if (!base_t::is_hooked()) base_t::set_hook(
		[](const node& v, const tref* ch, size_t len, tref r) {
			return get_hook<node>{}(v, ch, len, r);
		});
	// We only propagate the type information up: an untyped node takes
	// the type of its first typed child. Suppressing bool-child
	// propagation would need bool_type_id here, which calls back into
	// this get() -- a circular dependency.
	if (v.nt != wff && v.nt != ref_args && v.nt != fp_fallback) {
		size_t ba_type = get_type(v, ch, len);
		return base_t::get(v.ba_retype(ba_type), ch, len, r);
	}
	return base_t::get(v, ch, len, r);
}

template <NodeType node>
tref tree<node>::get(tref n, tref r) {
	return base_t::get(n, r);
}

template <NodeType node>
tref tree<node>::replace_value(const node& v) const {
	return base_t::replace_value(v);
}

template <NodeType node>
tref tree<node>::get(const node& v, const trefs& ch, tref r) {
	return get(v, ch.data(), ch.size(), r);
}

template <NodeType node>
tref tree<node>::get(const node& v, const std::initializer_list<tref>& ch, tref r) {
	return get(v, std::data(ch), ch.size(), r);
}

//------------------------------------------------------------------------------
// creation with node childs

template <NodeType node>
tref tree<node>::get(const node& v, const node& child) {
	return get(v, get(child));
}

template <NodeType node>
tref tree<node>::get(const node& v, const node& ch1, const node& ch2)
{
	return get(v, get(ch1), get(ch2));
}

template <NodeType node>
tref tree<node>::get(const node& v, const node* ch, size_t len, tref r)
{
	trefs nch;
	for (size_t i = 0; i < len; ++i) nch.push_back(get(ch[i]));
	return get(v, std::data(nch), len, r);
}

template <NodeType node>
tref tree<node>::get(const node& v, const std::vector<node>& ch, tref r)
{
	return get(v, ch.data(), ch.size(), r);
}

template <NodeType node>
tref tree<node>::get(const node& v, const std::initializer_list<node>& ch, tref r)
{
	return get(v, std::data(ch), ch.size(), r);
}

//------------------------------------------------------------------------------
// creation of node::type and children

template <NodeType node>
tref tree<node>::get(const node::type& nt) { return get(node(nt)); }

template <NodeType node>
tref tree<node>::get(const node::type& nt, tref ch) {
	return get(node(nt), ch);
}

template <NodeType node>
tref tree<node>::get(const node::type& nt, tref ch1, tref ch2) {
	return get(node(nt), ch1, ch2);
}

template <NodeType node>
tref tree<node>::get(const node::type& nt, const tref* ch, size_t len, tref r) {
	return get(node(nt), ch, len, r);
}

template <NodeType node>
tref tree<node>::get(const node::type& nt, const trefs& ch, tref r) {
	return get(node(nt), ch, r);
}

template <NodeType node>
tref tree<node>::get(const node::type& nt,
	const std::initializer_list<tref>& ch, tref r)
{
	return get(node(nt), ch, r);
}

template <NodeType node>
tref tree<node>::get(const node::type& nt, const std::string& str) {
	return get(node(nt, dict(str)));
}

template<NodeType node>
tref tree<node>::get_typed(const typename node::type& nt, size_t ba_type_id) {
	return get(node::ba_typed(nt, ba_type_id));
}

template<NodeType node>
tref tree<node>::get_typed(const typename node::type& nt, tref ch,
	size_t ba_type_id) {
	return get(node::ba_typed(nt, ba_type_id), ch);
}

template<NodeType node>
tref tree<node>::get_typed(const typename node::type& nt, tref ch1, tref ch2,
	size_t ba_type_id) {
	return get(node::ba_typed(nt, ba_type_id), ch1, ch2);
}

template<NodeType node>
tref tree<node>::get_typed(const typename node::type& nt, const tref* ch,
			size_t len, size_t ba_type_id, tref r) {
	return get(node::ba_typed(nt, ba_type_id), ch, len, r);
}

template<NodeType node>
tref tree<node>::get_typed(const typename node::type& nt, const trefs& ch,
			size_t ba_type_id, tref r) {
	return get(node::ba_typed(nt, ba_type_id), ch, r);
}

template<NodeType node>
tref tree<node>::get_typed(const typename node::type& nt,
			const std::initializer_list<tref>& ch, size_t ba_type_id, tref r) {
	return get(node::ba_typed(nt, ba_type_id), ch, r);
}

template<NodeType node>
tref tree<node>::get_typed(const typename node::type& nt,
	const std::string& str, size_t ba_type_id) {
	return get(node(nt, dict(str), true, ba_type_id));
}

//------------------------------------------------------------------------------
// terminals

template <NodeType node>
tref tree<node>::get_num(uint64_t n) {
	return get(node(tree<node>::num, n));
}

template <NodeType node>
tref tree<node>::get_integer(int_t n) {
	return get(node(integer, node::integer_data(n)));
}

// constants

template <NodeType node>
tref tree<node>::get_ba_constant(const constant& constant, tref type_tree)
{
	return ba_constants<node>::get(constant,
				       get_ba_type_id<node>(type_tree));
}

template <NodeType node>
tref tree<node>::get_ba_constant(const constant& constant, size_t ba_type_id)
{
	LOG_TRACE << " -- get ba_constant(constant constant, size_t ba_type_id): `"
		<< LOG_BA(constant) << "`, " << LOG_BA_TYPE(ba_type_id);
	return ba_constants<node>::get(constant, ba_type_id);
}

template <NodeType node>
result<tref> tree<node>::get_ba_constant(
	const std::string& constant_source,
	tref type_tree)
{
	return get_ba_constant_from_source(dict(constant_source),
		get_ba_type_id<node>(type_tree));
}

template <NodeType node>
result<tref> tree<node>::get_ba_constant_from_source(
	size_t constant_source_sid,
	size_t ba_type_id)
{
	result<tref> r;
	TAU_TRY(tref type_tree, ba_types<node>::type_tree(ba_type_id));
#ifdef DEBUG
	LOG_TRACE << " -- get ba_constant_from_source(size_t sid, size_t tid): `"
				<< dict(constant_source_sid) << "`, "
				<< LOG_BA_TYPE(ba_type_id) << " " << ba_type_id;
	if (ba_type_id == 0)
		LOG_TRACE << " -- untyped: " << dict(constant_source_sid);
	// TODO (HIGH) dropped error: ba_types::name's report -- a DEBUG-only trace line cannot carry it.
	else LOG_TRACE << " -- typed: "
		<< ba_types<node>::name(ba_type_id).value_or(std::string());
	assert(ba_type_id > 0);
#endif // DEBUG

	auto cnst_r = ba_constants<node>::get(dict(constant_source_sid), type_tree);
	tref value = cnst_r.has_value()
		? get_ba_constant(cnst_r.value().first, type_tree)
		: nullptr;
	// TODO (HIGH) dropped error: ba_constants::get's report -- the LOG_ERROR line cannot abort for it.
	if (value == nullptr) LOG_ERROR << "Parsing constant `"
		<< dict(constant_source_sid) << "` failed for type `"
		<< ba_types<node>::name(ba_type_id).value_or(std::string())
		<< "` (valid: " << node::ba::types_joined() << ").";
	else LOG_TRACE << " -- result: " << LOG_FM(value);
	return r.with_value(value);
}

template <NodeType node>
result<tref> tree<node>::get_ba_constant(size_t constant_id, size_t ba_type_id) {
	result<tref> r;
	TAU_TRY(constant c, ba_constants<node>::get(constant_id));
	LOG_TRACE << " -- get_ba_constant(size_t constant_id, size_t ba_type_id): `"
		<< LOG_BA(c) << "`, " << LOG_BA_TYPE(ba_type_id);
	return r.with_value(get_ba_constant(c, ba_type_id));
}

template <NodeType node>
tref tree<node>::get_ba_constant(
	const std::pair<constant, tref>& typed_const)
{
	LOG_TRACE << " -- get_ba_constant(pair<constant, string>): `"
		<< LOG_BA(typed_const.first) << "`, " << LOG_BA(typed_const.second);
	return get_ba_constant(typed_const.first,
			       get_ba_type_id<node>(typed_const.second));
}

template <NodeType node>
tref tree<node>::get_ba_constant(
	const std::optional<std::pair<constant, tref>>& typed_const)
{
	if (!typed_const) LOG_TRACE
		<< "get_ba_constant(optional): nullptr";
	if (!typed_const) return nullptr;
	return get_ba_constant(typed_const.value());
}


// -----------------------------------------------------------------------------
// children

template <NodeType node>
size_t tree<node>::children_size() const {
	return base_t::children_size();
}

template <NodeType node>
bool tree<node>::get_children(tref *child, size_t& len) const {
	return base_t::get_children(child, len);
}

template <NodeType node>
tref_range<node> tree<node>::children() const {
	return base_t::children();
}

template <NodeType node>
tree_range<tree<node>> tree<node>::children_trees() const{
	return tree_range<tree<node>>(this->l);
}

template <NodeType node>
tref tree<node>::only_child() const {
	return base_t::only_child();
}

template<NodeType node>
bool tree<node>::has_child() const {
	return base_t::has_child();
}

template <NodeType node>
tref tree<node>::trim() const { return first(); }

template <NodeType node>
tref tree<node>::trim(tref t) { return get(t).first(); }

template <NodeType node>
tref tree<node>::trim2() const { return first_tree().first(); }

template <NodeType node>
tref tree<node>::trim2(tref t) { return get(t)[0].first(); }

template <NodeType node>
trefs tree<node>::get_children() const {
	return base_t::get_children();
}

template <NodeType node>
tref tree<node>::child(size_t n) const { return base_t::child(n);}

template <NodeType node>
tref tree<node>::first() const { return child(0); }

template <NodeType node>
tref tree<node>::second() const { return child(1); }

template <NodeType node>
tref tree<node>::third() const { return child(2); }

template <NodeType node>
const tree<node>& tree<node>::child_tree(size_t n) const {
	tref c = child(n); DBG(assert(c != nullptr);)
	return get(c);
}

template <NodeType node>
typename tree<node>::traverser tree<node>::operator()() const {
	return traverser(get());
}

template <NodeType node>
const tree<node>& tree<node>::operator[](size_t n) const {
	return child_tree(n);
}

template <NodeType node>
const tree<node>& tree<node>::first_tree() const {
	return child_tree(0);
}

template <NodeType node>
const tree<node>& tree<node>::second_tree() const {
	return child_tree(1);
}

template <NodeType node>
const tree<node>& tree<node>::third_tree() const {
	return child_tree(2);
}

template <NodeType node>
const tree<node>& tree<node>::only_child_tree() const {
	tref c = only_child(); DBG(assert(c != nullptr);)
	return get(c);
}

template <NodeType node>
const tree<node>& tree<node>::right_sibling_tree() const {
	tref s = this->right_sibling(); DBG(assert(s != nullptr);)
	return get(s);
}

template <NodeType node>
tref tree<node>::reget(tref n) {
	auto reget_node = [](tref n) -> tref {
		const auto& t = tree<node>::get(n);
		trefs children;
		for (tref child : t.children())
			children.push_back(child);
		return tree<node>::get(t.value, children, t.right_sibling());
	};
	return post_order<node>(n).apply_unique(reget_node);
}

//------------------------------------------------------------------------------
// nt category helpers

template <NodeType node>
bool tree<node>::is_string_nt(size_t nt) {
	static const std::set<size_t> string_nts{
		sym, type, type_name, new_type_name, source, capture,
		var_name, uconst_name, file_name,
		ctnvar, option_name, option_value, member_name,
	};
	return string_nts.contains(nt);
}

template <NodeType node>
bool tree<node>::is_digital_nt(size_t nt) {
	static const std::set<size_t> digital_nts{
		num, history_id
	};
	return digital_nts.contains(nt);
}

template <NodeType node>
bool tree<node>::is_term_nt(size_t nt) {
	// Deliberately wider than the free idni::tau_lang::is_term_nt used by
	// node's constructor (see tau_tree_node.tmpl.h): this is the full
	// is-a-term predicate used by is_term(), covering the extended bf
	// arithmetic/functional operators and capture as well.
	switch (nt) {
		case bf:
		case ba_constant:
		case bf_fall:
		case bf_fex:
		case bf_ref:
		case bf_or:
		case bf_nor:
		case bf_xor:
		case bf_xnor:
		case bf_add:
		case bf_sub:
		case bf_mul:
		case bf_div:
		case bf_mod:
		case bf_shr:
		case bf_shl:
		case bf_min:
		case bf_max:
		case bf_and:
		case bf_nand:
		case bf_neg:
		case bf_cast:
		case bf_t:
		case bf_f:
		case variable:
		case capture: // capture only appears under terms now, otherwise error
			return true;
		default: return false;
	}
}

template <NodeType node>
bool tree<node>::is_term_nt(size_t nt, size_t parent_nt) {
	switch (nt) {
		case ref: return parent_nt == bf_ref;
		default: return is_term_nt(nt);
	}
}

//------------------------------------------------------------------------------
// fast access helpers

template <NodeType node>
uint64_t tree<node>::data() const {
	return this->value.get_data();
}

template <NodeType node>
uint64_t tree<node>::child_data() const {
	return first_tree().value.get_data();
}

template <NodeType node>
bool tree<node>::is(size_t nt) const {
	return this->value.nt == nt;
}

template <NodeType node>
bool tree<node>::is_string() const { return is_string_nt(this->value.nt); }

template <NodeType node>
bool tree<node>::is_integer() const { return is(integer); }

template <NodeType node>
bool tree<node>::is_num() const { return is(num) || is(history_id); }

template <NodeType node>
bool tree<node>::is_ba_constant() const { return is(ba_constant); }

template <NodeType node>
bool tree<node>::is_term() const { return is_term_nt(this->value.nt) || is(io_var); }

template <NodeType node>
bool tree<node>::is_input_variable() const {
	auto x = tt(*this);
	if (x.is(bf)) x = x | variable;
	if (x && x.is(variable)) x = x | io_var;
	return x && x.is(io_var) && (x | tt::data) == 1;
}

template <NodeType node>
bool tree<node>::is_output_variable() const {
	auto x = tt(*this);
	if (x.is(bf)) x = x | variable;
	if (x && x.is(variable)) x = x | io_var;
	return x && x.is(io_var) && (x | tt::data) == 2;
}

template <NodeType node>
bool tree<node>::equals_0() const {
	return is_child<node>(get(), tau::bf_f);
}

template <NodeType node>
bool tree<node>::equals_1() const {
	return is_child<node>(get(), tau::bf_t);
}

template <NodeType node>
bool tree<node>::equals_F() const {
	return is_child<node>(get(), tau::wff_f);
}

template <NodeType node>
bool tree<node>::equals_T() const {
	return is_child<node>(get(), tau::wff_t);
}

template <NodeType node>
bool tree<node>::child_is(size_t nt) const {
	if (!has_child()) return false;
	return first_tree().is(nt);
}

template <NodeType node>
node::type tree<node>::get_type() const {
	return static_cast<node::type>(this->value.nt);
}

template <NodeType node>
const std::string& tree<node>::get_type_name() const {
	return this->value.name();
}

template <NodeType node>
const std::string& tree<node>::get_string() const {
	DBG(assert(is_string());)
	return dict(get_string_id());
}

template <NodeType node>
size_t tree<node>::get_string_id() const {
	DBG(assert(is_string());)
	// The pool id is stored at full width; size_t indexes the pool.
	const uint64_t sid = this->data();
	DBG(assert(sid <= static_cast<uint64_t>(
		std::numeric_limits<size_t>::max()));)
	return static_cast<size_t>(sid);
}

template <NodeType node>
int_t tree<node>::get_integer() const {
	DBG(assert(is_integer());)
	return static_cast<int_t>(this->value.data);
}

template <NodeType node>
uint64_t tree<node>::get_num() const {
	DBG(assert(is_num());)
	return this->value.get_data();
}

template <NodeType node>
size_t tree<node>::get_ba_constant_id() const {
	DBG(LOG_TRACE << LOG_FM_TREE(get()));
	DBG(assert(is_ba_constant());)
	// ba_constants indexes its pool with size_t.
	const size_t id = static_cast<size_t>(this->data());
	return id;
}

template <NodeType node>
tree<node>::constant tree<node>::get_ba_constant() const {
	DBG(assert(is_ba_constant());)
	// TODO (HIGH) dropped error: ba_constants::get's report -- get_ba_constant returns a plain constant, which cannot carry it.
	// ba_constants indexes its pool with size_t.
	const size_t id = static_cast<size_t>(this->data());
	return ba_constants<node>::get(id).value_or(constant{});
}

template <NodeType node>
size_t tree<node>::get_ba_type() const {
	// Commented as now we also type bf nodes with ba_type
	//if (this->value.nt == bf)
	//	return child_tree(0).get_ba_type();
	return this->value.ba_type;
}

template<NodeType node>
result<tref> tree<node>::get_ba_type_tree() const {
	return ba_types<node>::type_tree(this->get_ba_type());
}

template <NodeType node>
const trefs& tree<node>::get_free_vars() const {
	return tau_lang::get_free_vars<node>(get());
}

template<NodeType node>
tref tree<node>::untype(tref term) {
	const tau& n = tau::get(term);
	trefs ch;
	for (auto c : n.get_children())
		if (!tau::get(c).is(tau::typed)) ch.push_back(c);
	// A ba_constant keeps its type: its data field is always a BA
	// constants pool index (node::hashit reads the pool unconditionally),
	// and constants of different types are distinct
	// values anyway, so there is nothing to erase -- clearing the type
	// would only detach the node from its algebra.
	auto retyped = n.is(tau::ba_constant) ? n.value
		: n.value.ba_retype(untyped_type_id<node>());
	return ch.empty() ? tau::get(retyped) : tau::get(retyped, ch);
}

template<NodeType node>
tref tree<node>::substitute(tref that, tref with) const {
	// If the replacement subtree 'with' contains a quantifier, the quantifier
	// ids need to be recalculated after substitution; otherwise just use simple replace.
	if (tau::get(with).find_top(is_logical_or_functional_quant<node>)) {
		return canonize_quantifier_ids<node>(this->replace(that, with));
	} else return this->replace(that, with);
}

template<NodeType node>
tref tree<node>::substitute(const auto& changes) const {
	bool canonize = false;
	for (auto& [k, v] : changes) {
		if (tau::get(v).find_top(is_logical_or_functional_quant<node>))
			canonize = true;
	}
	if (canonize) {
		return canonize_quantifier_ids<node>(this->replace(changes));
	} else return this->replace(changes);
}


template<NodeType node>
tref untype(tref term) {
	return tree<node>::untype(term);
}

template <NodeType node>
tref substitute(tref formula, tref that, tref with) {
	return tree<node>::get(formula).substitute(that, with);
}

template<NodeType node>
tref substitute(tref formula, const auto& changes) {
	return tree<node>::get(formula).substitute(changes);
}

} // namespace idni::tau_lang

template<typename... BAs> requires idni::tau_lang::BAsPack<BAs...>
size_t std::hash<idni::tau_lang::node<BAs...>>::operator()(
	const idni::tau_lang::node<BAs...>& n) const noexcept {
	// A 64-bit hash mixed into a size_t bucket loses bits on purpose.
	return static_cast<size_t>(n.hash);
}
