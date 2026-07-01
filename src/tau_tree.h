// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file tau_tree.h
 * @brief Central AST type and associated traversal/builder/printer/query API for Tau.
 *
 * Defines:
 * - `BAsPack` / `NodeType` concepts
 * - `node<BAs...>` — packed bit-field tree node
 * - `tree<node>` — Tau formula AST, extending `lcrs_tree<node>`
 *   with traversal (`traverser`), builders (`build_wff_*`, `build_bf_*`, etc.),
 *   pretty-printing, and query helpers
 * - Printer, query and printer free functions
 * - `std::hash<node<BAs...>>` specialization
 */

#ifndef __IDNI__TAU__TAU_TREE_H__
#define __IDNI__TAU__TAU_TREE_H__

#include <concepts>
#include <string>
#include <initializer_list>
#include <type_traits>


#include "defs.h"
#include "boolean_algebras/cvc5/cvc5.h"
#include "tau_parser.generated.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// concepts

/** @brief Helper: `true` if `T` appears in the pack `Ts...`. */
template <typename T, typename... Ts>
constexpr bool pack_contains = (std::is_same_v<T, Ts> || ...);

/**
 * @brief Concept satisfied by a non-empty BA pack that contains `cvc5::Term`.
 *
 * Ensures every supported BA combination includes a CVC5 backend for
 * bitvector reasoning.
 */
template <typename... BAs>
concept BAsPack = sizeof...(BAs) > 0 && pack_contains<cvc5::Term, BAs...>;

/**
 * @brief Concept satisfied by a valid Tau AST node type.
 *
 * Requires:
 * - `node::type` — nonterminal enum (`tau_parser::nonterminal`)
 * - `node::constant` — BA variant type alias
 * - `node::ba` — dispatcher type alias
 * - `nt`, `term`, `ba_type`, `data` fields convertible to integral types
 */
template <typename node>
concept NodeType = requires {
	typename node::type;
	typename node::constant;
	typename node::ba;
	{ std::declval<node>().nt } -> std::convertible_to<size_t>;
	{ std::declval<node>().term } -> std::convertible_to<bool>;
	{ std::declval<node>().ba_type } -> std::convertible_to<size_t>;
	{ std::declval<node>().data } -> std::convertible_to<size_t>;
};

// -----------------------------------------------------------------------------
// forward declarations

template <NodeType node> struct rr;
template <NodeType node> struct ba_constants;
template <typename... BAs> requires BAsPack<BAs...> struct base_ba_dispatcher;
template <typename... BAs> requires BAsPack<BAs...> struct tau_ba;
template <NodeType node> struct io_context;
template <NodeType node> struct tau_spec;

/**
 * @brief Packed bit-field AST node for Tau formula trees.
 *
 * A `node` encodes the nonterminal id (`nt`), a `term` flag, an extension
 * flag (`ext`), inline data (`data`), and a BA type id (`ba_type`) into a
 * single `size_t`-sized bit-field for cache efficiency.
 *
 * `node` also exports type aliases (`constant`, `ba`) so they are accessible
 * everywhere a `node` type parameter is in scope, avoiding repeated template
 * argument threading.
 *
 * @tparam BAs Pack of Boolean algebra types satisfying `BAsPack`.
 */
template <typename... BAs>
requires BAsPack<BAs...>
struct node {
	using node_t = node<BAs...>;
	using type = tau_parser::nonterminal;   ///< Nonterminal enum type.
	using constant = std::variant<BAs...>;  ///< BA constant variant type.
	using constant_with_type = std::pair<constant, tref>;
	using ba = tau_lang::base_ba_dispatcher<BAs...>; ///< BA dispatcher alias.

	using T = size_t;

	// bit sizes
	static constexpr size_t bits      = std::numeric_limits<T>::digits;
	static constexpr size_t nt_bits   = tau_parser_data::nt_bits;
	static constexpr size_t data_bits = bits - nt_bits - 1;

	// masks and shifts
	static constexpr size_t nt_mask = (size_t(1) << nt_bits) - size_t(1);
	static constexpr size_t data_mask = (size_t(1) << data_bits) - size_t(1);
	static constexpr size_t ext_shift = data_bits;
	static constexpr size_t term_shift = ext_shift + 1;
	static constexpr size_t nt_shift = term_shift + 1;

	const T nt   : nt_bits   = 0; ///< Nonterminal id.
	const T term : 1         = 0; ///< 1 = is term (bf), 0 = is formula (wff).
	const T ext  : 1         = 0; ///< 1 = data stored in child (not yet used).
	const T data : data_bits = 0; ///< Inline data (meaning depends on `nt`).
	const unsigned short ba_type; ///< Boolean-algebra type id.
	const size_t hash;            ///< Pre-computed hash for fast equality checks.

	/**
	 * @brief Generic constructor.
	 * @param nt Nonterminal id.
	 * @param data Inline data payload.
	 * @param is_term 1 if this node is a term.
	 * @param ba_type BA type id.
	 * @param ext Extension flag (unused, always 0).
	 */
	constexpr node(size_t nt = 0, size_t data = 0, size_t is_term = 0,
			size_t ba_type = 0, size_t ext = 0) noexcept;

	/** @brief Return a copy of this node with the nonterminal changed to @p new_nt. */
	node retype(size_t new_nt) const;
	/** @brief Return a copy with the BA type id changed to @p new_ba. */
	node ba_retype(size_t new_ba) const;
	/** @brief Return a copy with the inline data changed to @p new_data. */
	node replace_data(size_t new_data) const;

	/** @brief Construct a BA-constant node with value index @p v and type @p ba_tid. */
	static constexpr node_t ba_constant(size_t v, size_t ba_tid = 0);

	/** @brief Construct a BA-typed terminal node of type @p nt with BA type @p ba_tid. */
	static constexpr node_t ba_typed(type nt, size_t ba_tid = 0);

	/** @brief Construct an input I/O-variable node with type @p ba_tid. */
	static constexpr node_t input_variable(size_t ba_tid = 0);
	/** @brief Construct an output I/O-variable node with type @p ba_tid. */
	static constexpr node_t output_variable(size_t ba_tid = 0);

	/** @brief Return the string name of nonterminal @p nt. */
	static const std::string& name(size_t nt);
	/** @brief Return the string name of this node's nonterminal. */
	const std::string& name() const;

	/** @brief Reinterpret the inline `data` field as `int_t`. */
	int_t as_int() const;

	/** @brief Return a null sentinel node. */
	static constexpr node_t nnull();

	/** @brief Construct an extension node from raw storage @p raw_value. */
	static constexpr node_t extension(T raw_value);
	/** @brief Return the raw storage value of an extension node. */
	constexpr T extension() const noexcept;

	/** @brief Three-way comparison (by hash, then field-by-field). */
	std::weak_ordering operator<=>(const node& that) const;
	/** @brief Strict less-than. */
	constexpr bool     operator<  (const node& that) const;
	/** @brief Less-than-or-equal. */
	constexpr bool     operator<= (const node& that) const;
	/** @brief Greater-than. */
	constexpr bool     operator>  (const node& that) const;
	/** @brief Greater-than-or-equal. */
	constexpr bool     operator>= (const node& that) const;
	/** @brief Equality. */
	constexpr auto     operator== (const node& that) const;
	/** @brief Inequality. */
	constexpr auto     operator!= (const node& that) const;

	/** @brief Return the pre-computed hash value for this node. */
	constexpr size_t hashit() const;
};

struct tau_parser_nonterminal_groups {
	enum nonterminal {
		typeable_symbol = 1000
		// quantifiers
		// atomic...
	};
};
/**
 * @brief Tau formula AST, extending `lcrs_tree<node>` with Tau-specific operations.
 *
 * `tree<node>` is the central data structure for Tau terms and formulas.
 * It is typically created through the static `get(...)` factory overloads or
 * transformed from a `tau_parser::tree` parse result.
 *
 * Key sub-APIs (each in its own `.tmpl.h` implementation file):
 * - **Direct tree API** (`get`, `child`, `trim`, etc.)
 * - **Traverser** (`traverser` / `tt`) — fluent tree navigation via `|` / `||`
 * - **Builders** (`build_wff_*`, `build_bf_*`, etc.) — formula construction
 * - **From-parser** (`get(string)`, `get(parse_result)`) — parse-to-tree conversion
 *
 * @tparam node Tree node type satisfying `NodeType` (usually `node<BAs...>`).
 */
template <NodeType node>
struct tree : public lcrs_tree<node>, public tau_parser_nonterminals,
	    public tau_parser_nonterminal_groups {
	using node_type = node;
	using base_t = lcrs_tree<node>;
	using parse_tree = tau_parser::tree;
	using tau = tree<node>;
	using constant = node::constant;
	using constant_with_type = node::constant_with_type;

	struct get_options; // fwd

	// -----------------------------------------------------------------------
	// Direct tree API
	// -----------------------------------------------------------------------

	/** @brief Return a `tref` for this node instance. */
	tref get() const;
	/** @brief Return the `tree` object for @p id. */
	static const tree& get(const tref id);
	/** @brief Return the `tree` object for handle @p h. */
	static const tree& get(const htref& h);
	/** @brief Return a handle ref for @p id. */
	static htref geth(tref id);
	/** @brief Return a handle ref for @p n. */
	static htref geth(const tree& n);

	/** @brief Transform parse tree @p t to a `tref` using @p options. */
	static tref get(const tau_parser::tree& t, get_options& options);
	/** @brief Transform parse tree @p t to a `tref` using @p options (rvalue). */
	static tref get(const tau_parser::tree& t, get_options&& options);
	/** @brief Re-register tree @p n in post-order (useful after structural edits). */
	static tref reget(tref n);

	/** @brief Create node @p v with children @p ch without triggering hooks. */
	static tref get_raw(const node& v, const tref* ch = nullptr,
					size_t len = 0, tref r = nullptr);

	/** @brief Return a copy of @p n with right sibling updated to @p r. */
	static tref get(tref n, tref r);

	/** @brief Return a copy of this node with its value replaced by @p v. */
	tref replace_value(const node& v) const;

	/** @brief Create a leaf node with value @p v. */
	static tref get(const node& v);
	/** @brief Create node @p v with single child @p ch. */
	static tref get(const node& v, tref ch);
	/** @brief Create node @p v with children @p ch1, @p ch2. */
	static tref get(const node& v, tref ch1, tref ch2);
	/** @brief Create node @p v with @p len children from array @p ch. */
	static tref get(const node& v, const tref* ch, size_t len, tref r = nullptr);
	/** @brief Create node @p v with children from vector @p ch. */
	static tref get(const node& v, const trefs& ch, tref r = nullptr);
	/** @brief Create node @p v with children from initializer list @p ch. */
	static tref get(const node& v, const std::initializer_list<tref>& ch, tref r = nullptr);

	/** @brief Create node @p v with single child node @p ch. */
	static tref get(const node& v, const node& ch);
	/** @brief Create node @p v with two child nodes @p ch1, @p ch2. */
	static tref get(const node& v, const node& ch1, const node& ch2);
	/** @brief Create node @p v with @p len child nodes from @p ch array. */
	static tref get(const node& v, const node* ch, size_t len, tref r = nullptr);
	/** @brief Create node @p v with children from @p ch vector. */
	static tref get(const node& v, const std::vector<node>& ch, tref r = nullptr);
	/** @brief Create node @p v with children from initializer list @p ch. */
	static tref get(const node& v, const std::initializer_list<node>& ch, tref r = nullptr);

	/** @brief Create a leaf of nonterminal type @p nt. */
	static tref get(const node::type& nt);
	/** @brief Create a node of type @p nt with single child @p ch. */
	static tref get(const node::type& nt, tref ch);
	/** @brief Create a node of type @p nt with children @p ch1, @p ch2. */
	static tref get(const node::type& nt, tref ch1, tref ch2);
	/** @brief Create a node of type @p nt with @p len children from @p ch. */
	static tref get(const node::type& nt, const tref* ch, size_t len, tref r = nullptr);
	/** @brief Create a node of type @p nt with children vector @p ch. */
	static tref get(const node::type& nt, const trefs& ch, tref r = nullptr);
	/** @brief Create a node of type @p nt with children from initializer list. */
	static tref get(const node::type& nt, const std::initializer_list<tref>& ch, tref r = nullptr);
	/** @brief Create a string terminal node of type @p nt. */
	static tref get(const node::type& nt, const std::string& str);

	/** @brief Create a typed leaf of type @p nt with BA type @p ba_type_id. */
	static tref get_typed(const node::type& nt, size_t ba_type_id);
	/** @brief Create a typed node with single child and BA type. */
	static tref get_typed(const node::type& nt, tref ch, size_t ba_type_id);
	/** @brief Create a typed node with two children and BA type. */
	static tref get_typed(const node::type& nt, tref ch1, tref ch2, size_t ba_type_id);
	/** @brief Create a typed node with @p len children from @p ch. */
	static tref get_typed(const node::type& nt, const tref* ch, size_t len,
				size_t ba_type_id, tref r = nullptr);
	/** @brief Create a typed node with children vector and BA type. */
	static tref get_typed(const node::type& nt, const trefs& ch, size_t ba_type_id, tref r = nullptr);
	/** @brief Create a typed node with children list and BA type. */
	static tref get_typed(const node::type& nt,
				const std::initializer_list<tref>& ch, size_t ba_type_id, tref r = nullptr);
	/** @brief Create a typed string terminal with BA type. */
	static tref get_typed(const node::type& nt, const std::string& str, size_t ba_type_id);

	/** @brief Create a `num` literal node from @p n. */
	static tref get_num(size_t n);
	/** @brief Create an `integer` literal node from @p i. */
	static tref get_integer(int_t i);

	/** @brief Create a BA-constant node from @p constant and type tree @p type_tree. */
	static tref get_ba_constant(const constant& constant, const tref type_tree);
	/** @brief Create a BA-constant node from @p constant and type id @p ba_type_id. */
	static tref get_ba_constant(const constant& constant, size_t ba_type_id);
	/** @brief Parse source string @p constant_source and create a BA-constant node. */
	static tref get_ba_constant(const std::string& constant_source, tref type_tree);
	/** @brief Create a BA-constant node from pre-interned source id and type id. */
	static tref get_ba_constant_from_source(size_t constant_source_sid, size_t ba_type_id);
	/** @brief Create a BA-constant node from pre-registered constant id and type id. */
	static tref get_ba_constant(size_t constant_id, size_t ba_type_id);
	/** @brief Create a BA-constant node from a typed-constant pair. */
	static tref get_ba_constant(const std::pair<constant, tref>& typed_const);
	/** @brief Create a BA-constant node from an optional typed-constant pair. */
	static tref get_ba_constant(const std::optional<std::pair<constant, tref>>& typed_const);
	/** @brief Create a bitvector constant node with @p bitwidth bits and value @p value. */
	static tref get_bv_constant(size_t bitwidth, size_t value);

	// -----------------------------------------------------------------------
	// Children
	// -----------------------------------------------------------------------
	/** @brief Return the number of direct children. */
	size_t children_size() const;
	/** @brief Fill @p ch array with up to @p len child refs; update @p len. */
	bool get_children(tref *ch, size_t& len) const;
	/** @brief Return child refs as a vector. */
	trefs get_children() const;
	/** @brief Return a lazy range over child refs. */
	tref_range<node> children() const;
	/** @brief Return a lazy range over child `tree` objects. */
	tree_range<tree> children_trees() const;

	/** @brief Return the @p n-th child ref (0-indexed). */
	tref child(size_t n) const;
	/** @brief Return the first child ref. */
	tref first()  const;
	/** @brief Return the second child ref. */
	tref second() const;
	/** @brief Return the third child ref. */
	tref third()  const;
	/** @brief Return the only child ref (asserts exactly one child). */
	tref only_child() const;
	/** @brief Return `true` if this node has at least one child. */
	bool has_child() const;

	/** @brief Return the first child's ref (shorthand for `first()`). */
	tref trim() const;
	/** @brief Return the first child's ref for @p t. */
	static tref trim(tref t);
	/** @brief Return the first child of the first child's ref. */
	tref trim2() const;
	/** @brief Return the first child of the first child's ref for @p t. */
	static tref trim2(tref t);

	/** @brief Return the @p n-th child as a `tree` reference. */
	const tree& operator[](size_t n) const;
	/** @brief Return the @p n-th child as a `tree` reference. */
	const tree& child_tree(size_t n) const;
	/** @brief Return the first child as a `tree` reference. */
	const tree& first_tree()  const;
	/** @brief Return the second child as a `tree` reference. */
	const tree& second_tree() const;
	/** @brief Return the third child as a `tree` reference. */
	const tree& third_tree()  const;
	/** @brief Return the only child as a `tree` reference. */
	const tree& only_child_tree() const;
	/** @brief Return the right sibling as a `tree` reference. */
	const tree& right_sibling_tree() const;

	// -----------------------------------------------------------------------
	// Printing / debugging
	// -----------------------------------------------------------------------
	/** @brief Pretty-print this node to @p o. */
	std::ostream& print(std::ostream& o) const;
	/** @brief Print the subtree structure to @p o, indented by @p s. */
	std::ostream& print_tree(std::ostream& o, size_t s = 0) const;
	/** @brief Return a pretty-printed string for this node. */
	std::string to_str() const;
	/** @brief Return the subtree structure as a string. */
	std::string tree_to_str() const;
	/** @brief Return a debug dump string for this subtree. */
	std::string dump_to_str(bool subtree = true) const;
	/** @brief Dump this subtree to @p os. */
	std::ostream& dump(std::ostream& os, bool subtree = true) const;
	/** @brief Dump this subtree to `std::cout`. */
	const tree& dump(bool subtree = true) const;
	/** @brief Dump subtree rooted at @p n to @p os. */
	static std::ostream& dump(std::ostream& os, tref n, bool subtree = true);

	// -----------------------------------------------------------------------
	// Node type category helpers
	// -----------------------------------------------------------------------
	/** @brief Return `true` if @p nt represents a numeric terminal. */
	static bool is_digital_nt(size_t nt);
	/** @brief Return `true` if @p nt represents a string terminal. */
	static bool is_string_nt(size_t nt);
	/** @brief Return `true` if @p nt is a term nonterminal under parent @p parent_nt. */
	static bool is_term_nt(size_t nt, size_t parent_nt);
	/** @brief Return `true` if @p nt is any term nonterminal. */
	static bool is_term_nt(size_t nt);

	// -----------------------------------------------------------------------
	// Fast access helpers
	// -----------------------------------------------------------------------
	/** @brief Return the inline `data` field of this node. */
	size_t data() const;
	/** @brief Return the inline `data` field of the first child. */
	size_t child_data() const;

	/** @brief Return `true` if this node's nonterminal id equals @p nt. */
	bool is(size_t nt) const;
	/** @brief Return `true` if this node stores a string. */
	bool is_string() const;
	/** @brief Return `true` if this node stores an integer. */
	bool is_integer() const;
	/** @brief Return `true` if this node stores a num literal. */
	bool is_num() const;
	/** @brief Return `true` if this is a BA-constant node. */
	bool is_ba_constant() const;
	/** @brief Return `true` if this is a term (bf) node. */
	bool is_term() const;
	/** @brief Return `true` if this is an input I/O variable. */
	bool is_input_variable() const;
	/** @brief Return `true` if this is an output I/O variable. */
	bool is_output_variable() const;

	/** @brief Return `true` if this node equals the BA zero constant. */
	bool equals_0() const;
	/** @brief Return `true` if this node equals the BA one constant. */
	bool equals_1() const;
	/** @brief Return `true` if this node equals the wff `F` constant. */
	bool equals_F() const;
	/** @brief Return `true` if this node equals the wff `T` constant. */
	bool equals_T() const;

	/** @brief Return `true` if the first child's nonterminal id equals @p nt. */
	bool child_is(size_t nt) const;

	/** @brief Return this node's nonterminal as a `node::type` enum value. */
	node::type get_type() const;
	/** @brief Return the string name of this node's nonterminal. */
	const std::string& get_type_name() const;
	/** @brief Return the string stored in this terminal node. */
	const std::string& get_string() const;
	/** @brief Return the integer stored in this terminal node. */
	int_t get_integer() const;
	/** @brief Return the num literal stored in this terminal node. */
	size_t get_num() const;
	/** @brief Return the BA-constant registry id for this node. */
	size_t get_ba_constant_id() const;
	/** @brief Return the BA constant value for this node. */
	constant get_ba_constant() const;
	/** @brief Return the BA type id for this node. */
	size_t get_ba_type() const;
	/** @brief Return the string name of this node's BA type. */
	std::string get_ba_type_name() const;
	/** @brief Return the BA type tree ref for this node. */
	tref get_ba_type_tree() const;
	/** @brief Return the free variables reachable from this node. */
	const trefs& get_free_vars() const;

	/** @brief Return a copy of @p term with all BA type annotations stripped. */
	static tref untype(tref term);

	/** @brief Replace @p that with @p with in this subtree. */
	tref substitute(tref that, tref with) const;
	/** @brief Apply all substitutions in @p changes to this subtree. */
	tref substitute(const auto& changes) const;

	// -----------------------------------------------------------------------
	// Predicate-based selection helpers
	// -----------------------------------------------------------------------

	/** @brief Select top-level nodes matching each of @p count predicates in @p queries. */
	std::vector<trefs> select_top_by_predicates(
		const std::function<bool(const tref&)>* queries, const size_t count) const;
	/** @brief Select top-level nodes matching each predicate in the list. */
	std::vector<trefs> select_top_by_predicates(
		const std::initializer_list<std::function<bool(const tref&)>>& queries) const;
	/** @brief Select top-level nodes matching each predicate in the vector. */
	std::vector<trefs> select_top_by_predicates(
		const std::vector<std::function<bool(const tref&)>>& queries) const;
	/** @brief Like `select_top_by_predicates` but stops at nodes satisfying @p until. */
	std::vector<trefs> select_top_until_by_predicates(
		const std::function<bool(const tref&)>* queries, const size_t count,
		const auto& until) const;
	/** @brief List variant of `select_top_until_by_predicates`. */
	std::vector<trefs> select_top_until_by_predicates(
		const std::initializer_list<std::function<bool(const tref&)>>& queries,
		const auto& until) const;
	/** @brief Vector variant of `select_top_until_by_predicates`. */
	std::vector<trefs> select_top_until_by_predicates(
		const std::vector<std::function<bool(const tref&)>>& queries,
		const auto& until) const;
	/** @brief Select all nodes (any depth) matching each predicate. */
	std::vector<trefs> select_all_by_predicates(
		const std::function<bool(const tref&)>* queries, const size_t count) const;
	/** @brief List variant of `select_all_by_predicates`. */
	std::vector<trefs> select_all_by_predicates(
		const std::initializer_list<std::function<bool(const tref&)>>& queries) const;
	/** @brief Vector variant of `select_all_by_predicates`. */
	std::vector<trefs> select_all_by_predicates(
		const std::vector<std::function<bool(const tref&)>>& queries) const;
	/** @brief Like `select_all_by_predicates` but stops at nodes satisfying @p until. */
	std::vector<trefs> select_all_until_by_predicates(
		const std::function<bool(const tref&)>* queries, const size_t count,
		const auto& until) const;
	/** @brief List variant of `select_all_until_by_predicates`. */
	std::vector<trefs> select_all_until_by_predicates(
		const std::initializer_list<std::function<bool(const tref&)>>& queries,
		const auto& until) const;
	/** @brief Vector variant of `select_all_until_by_predicates`. */
	std::vector<trefs> select_all_until_by_predicates(
		const std::vector<std::function<bool(const tref&)>>& queries,
		const auto& until) const;

	// -----------------------------------------------------------------------
	// From-parser API (tau_tree_from_parser.tmpl.h)
	// -----------------------------------------------------------------------

	/**
	 * @brief Options controlling parse-to-tree conversion.
	 */
	struct get_options {
		tau_parser::parse_options parse{};          ///< Underlying parser options.
		std::map<std::string, tref> named_constants{}; ///< Pre-bound constant names.
		bool infer_ba_types = true;                 ///< Run BA type inference.
		bool use_default_types = true;              ///< Fall back to tau type for unknowns.
		bool reget_with_hooks = true;               ///< Re-register nodes through hooks.
		const std::vector<htref>* definition_heads = nullptr; ///< Known definition heads.
		subtree_map<node, size_t>* global_scope = nullptr;    ///< Pre-known global types.
		io_context<node>* context = nullptr;        ///< I/O stream context.
	};

	/** @brief Convert parser @p result to a tree using @p options. */
	static tref get(tau_parser::result& result, get_options& options);
	/** @brief Convert parser @p result to a tree using @p options (rvalue). */
	static tref get(tau_parser::result& result, get_options&& options);
	/** @brief Parse @p str with default options. */
	static tref get(const std::string& str);
	/** @brief Parse @p str with @p options. */
	static tref get(const std::string& str, get_options& options);
	/** @brief Parse @p str with @p options (rvalue). */
	static tref get(const std::string& str, get_options&& options);
	/** @brief Parse from stream @p is with @p options. */
	static tref get(std::istream& is, get_options& options);
	/** @brief Parse from stream @p is with @p options (rvalue). */
	static tref get(std::istream& is, get_options&& options);
	/** @brief Parse file @p filename with @p options. */
	static tref get_from_file(const std::string& filename, get_options& options);
	/** @brief Parse file @p filename with @p options (rvalue). */
	static tref get_from_file(const std::string& filename, get_options&& options);

	// -----------------------------------------------------------------------
	// Traverser / tt API (tau_tree_traverser.tmpl.h)
	// -----------------------------------------------------------------------

	struct traverser; // forward declaration

	/**
	 * @brief Typed value extractor for `traverser` pipelines.
	 *
	 * Used via `tt | extractor<T>` to pull typed data out of a traversal.
	 * @tparam result_type The extracted value type.
	 */
	template <typename result_type>
	struct extractor {
		using function = std::function<result_type(const traverser&)>;
		/** @brief Construct from a callable @p e. */
		extractor(const function& e);
		/** @brief Invoke the extractor on traverser @p t. */
		result_type operator()(const traverser& t) const;
	private:
		function e;
	};

	/**
	 * @brief Fluent tree-navigation wrapper.
	 *
	 * A `traverser` wraps one or more `tref` values. Use `| nt` to descend to
	 * the first child with nonterminal @p nt, `|| nt` to collect all such
	 * children, and `| extractor<T>` to extract a typed value.
	 */
	struct traverser {
		using type = typename node::type;

		/** @brief Construct a null traverser. */
		traverser();
		/** @brief Construct a traverser wrapping a single ref @p r. */
		traverser(tref r);
		/** @brief Construct a traverser wrapping a tree @p r. */
		traverser(const tree& r);
		/** @brief Construct a traverser from an optional ref. */
		traverser(const std::optional<tref>& r);
		/** @brief Construct a traverser from a handle ref. */
		traverser(const htref& r);
		/** @brief Construct a traverser over multiple refs. */
		traverser(const trefs& n);

		/** @brief Return `true` if this traverser holds at least one value. */
		bool has_value() const;
		/** @brief Explicit bool conversion: `true` if non-empty. */
		explicit operator bool() const;
		/** @brief Return the single ref value (asserts exactly one). */
		tref value() const;
		/** @brief Return the single value as a `tree` reference. */
		const tree& value_tree() const;
		/** @brief Return the @p n-th child of the single value as a `tree`. */
		const tree& operator[](size_t n) const;
		/** @brief Return all held refs as a vector. */
		trefs values() const;
		/** @brief Return each held ref wrapped in its own traverser. */
		std::vector<traverser> traversers() const;
		/** @brief Alias for `traversers()`. */
		std::vector<traverser> operator()() const;
		/** @brief Return `true` if the single value's nonterminal id equals @p nt. */
		bool is(size_t nt) const;
		/** @brief Return `true` if this traverser holds no values. */
		bool empty() const;
		/** @brief Return the number of held values. */
		size_t size() const;

		// Pre-defined extractors
		static const extractor<tref>                ref;            ///< Extract the single `tref`.
		static const extractor<trefs>               refs;           ///< Extract all `tref` values.
		static const extractor<htref>               handle;         ///< Extract as `htref`.
		static const extractor<const tree<node>&>   Tree;           ///< Extract the `tree` reference.
		static const extractor<traverser>           dump;           ///< Dump and return `*this`.
		static const extractor<traverser>           print_tree;     ///< Print subtree and return `*this`.
		static const extractor<type>                nt;             ///< Extract the nonterminal type.
		static const extractor<std::string>         string;         ///< Extract stored string.
		static const extractor<int_t>               integer;        ///< Extract stored integer.
		static const extractor<size_t>              num;            ///< Extract stored num.
		static const extractor<size_t>              data;           ///< Extract inline data.
		static const extractor<size_t>              ba_constant_id; ///< Extract BA-constant id.
		static const extractor<size_t>              ba_type;        ///< Extract BA type id.
		static const extractor<constant>            ba_constant;    ///< Extract BA constant value.
		static const extractor<traverser>           only_child;     ///< Descend to only child.
		static const extractor<traverser>           first;          ///< Descend to first child.
		static const extractor<traverser>           second;         ///< Descend to second child.
		static const extractor<traverser>           third;          ///< Descend to third child.
		static const extractor<traverser>           fourth;         ///< Descend to fourth child.
		static const extractor<traverser>           children;       ///< Collect all children.
		static const extractor<tref_range<node>>    children_range; ///< Lazy child ref range.
		static const extractor<tree_range<tree<node>>> children_trees_range; ///< Lazy child tree range.
		/** @brief Create a function-wrapping extractor from @p fn. */
		static const extractor<traverser> f(const auto& fn);

		/** @brief Descend to the first child with nonterminal @p nt. */
		traverser operator|(size_t nt) const;
		/** @brief Collect all children with nonterminal @p nt. */
		traverser operator||(size_t nt) const;
		/** @brief Extract a typed value using @p e. */
		template <typename result_type>
		result_type operator|(const extractor<result_type>&) const;
		/** @brief Extract a typed value using @p e (alias for `operator|`). */
		template <typename result_type>
		result_type operator||(const extractor<result_type>&) const;
	private:
		void set_values(const trefs& n);
		bool has_value_ = true;
		trefs values_{};
	};

	/** @brief Wrap this node in a `traverser`. */
	traverser operator()() const;

	// -----------------------------------------------------------------------
	// Builder API (tau_tree_builders.tmpl.h)
	// -----------------------------------------------------------------------

	/** @brief Apply rewriter builder @p b to nodes @p n. */
	static tref apply_builder(const rewriter::builder& b, trefs n);

	/** @brief Return the BA zero constant for type @p type_id. */
	static tref _0(size_t type_id);
	/** @brief Return the BA one constant for type @p type_id. */
	static tref _1(size_t type_id);
	/** @brief Return the wff `F` (false) constant. */
	static tref _F();
	/** @brief Return the wff `T` (true) constant. */
	static tref _T();
	/** @brief Return the trimmed (child-only) BA zero for @p type_id. */
	static tref _0_trimmed(size_t type_id);
	/** @brief Return the trimmed BA one for @p type_id. */
	static tref _1_trimmed(size_t type_id);
	/** @brief Return the trimmed wff `F`. */
	static tref _F_trimmed();
	/** @brief Return the trimmed wff `T`. */
	static tref _T_trimmed();
	/** @brief Return the `tree` for BA zero of @p type_id. */
	static const tree<node>& get_0(size_t type_id);
	/** @brief Return the `tree` for BA one of @p type_id. */
	static const tree<node>& get_1(size_t type_id);
	/** @brief Return the `tree` for wff `F`. */
	static const tree<node>& get_F();
	/** @brief Return the `tree` for wff `T`. */
	static const tree<node>& get_T();
	/** @brief Return the trimmed `tree` for BA zero of @p type_id. */
	static const tree<node>& get_0_trimmed(size_t type_id);
	/** @brief Return the trimmed `tree` for BA one of @p type_id. */
	static const tree<node>& get_1_trimmed(size_t type_id);
	/** @brief Return the trimmed `tree` for wff `F`. */
	static const tree<node>& get_F_trimmed();
	/** @brief Return the trimmed `tree` for wff `T`. */
	static const tree<node>& get_T_trimmed();

	// Formula builders
	/** @brief Build `sometimes n`. */
	static tref build_wff_sometimes(tref n);
	/** @brief Build `always n`. */
	static tref build_wff_always(tref n);
	/** @brief Build `x ? y : z` conditional. */
	static tref build_wff_conditional(tref x, tref y, tref z);
	/** @brief Build `all bound_var. subformula`. */
	static tref build_wff_all(tref bound_var, tref subformula,
		bool calculate_quant_id = true);
	/** @brief Build `all bound_vars[0], ..., bound_vars[n-1]. subformula`. */
	static tref build_wff_all_many(const trefs& bound_vars, tref subformula);
	/** @brief Build `ex bound_var. subformula`. */
	static tref build_wff_ex(tref bound_var, tref subformula,
		bool calculate_quant_id = true);
	/** @brief Build `ex bound_vars[0], ..., bound_vars[n-1]. subformula`. */
	static tref build_wff_ex_many(const trefs& bound_vars, tref subformula);
	/** @brief Build `l -> r`. */
	static tref build_wff_imply(tref l, tref r);
	/** @brief Build `l <- r`. */
	static tref build_wff_rimply(tref l, tref r);
	/** @brief Build `l <-> r`. */
	static tref build_wff_equiv(tref l, tref r);
	/** @brief Build `l | r`. */
	static tref build_wff_or(tref l, tref r);
	/** @brief Build disjunction over all formulas in @p wffs. */
	static tref build_wff_or(const auto& wffs);
	/** @brief Build `l ^ r`. */
	static tref build_wff_xor(tref l, tref r);
	/** @brief Build `l & r`. */
	static tref build_wff_and(tref l, tref r);
	/** @brief Build conjunction over all formulas in @p wffs. */
	static tref build_wff_and(const auto& wffs);
	/** @brief Build `! n`. */
	static tref build_wff_neg(tref n);

	// Constraint builders
	/** @brief Build `ctnvar != num` constraint. */
	static tref build_wff_ctn_neq(tref ctnvar, tref num);
	/** @brief Build `ctnvar == num` constraint. */
	static tref build_wff_ctn_eq(tref ctnvar, tref num);
	/** @brief Build `ctnvar >= num` constraint. */
	static tref build_wff_ctn_gteq(tref ctnvar, tref num);
	/** @brief Build `ctnvar > num` constraint. */
	static tref build_wff_ctn_gt(tref ctnvar, tref num);
	/** @brief Build `ctnvar <= num` constraint. */
	static tref build_wff_ctn_lteq(tref ctnvar, tref num);
	/** @brief Build `ctnvar < num` constraint. */
	static tref build_wff_ctn_lt(tref ctnvar, tref num);

	// Term relational builders (wff using bf)
	/** @brief Build `x[y, z]` interval formula. */
	static tref build_bf_interval(tref x, tref y, tref z);
	/** @brief Build `l = r`. */
	static tref build_bf_eq(tref l, tref r);
	/** @brief Build `l = 0`. */
	static tref build_bf_eq_0(tref l);
	/** @brief Build `l = 1`. */
	static tref build_bf_eq_1(tref l);
	/** @brief Build `l != r`. */
	static tref build_bf_neq(tref l, tref r);
	/** @brief Build `l != 0`. */
	static tref build_bf_neq_0(tref l);
	/** @brief Build `l <= r`. */
	static tref build_bf_lteq(tref l, tref r);
	/** @brief Build `!(l <= r)`. */
	static tref build_bf_nlteq(tref l, tref r);
	/** @brief Build `l > r`. */
	static tref build_bf_gt(tref l, tref r);
	/** @brief Build `!(l > r)`. */
	static tref build_bf_ngt(tref l, tref r);
	/** @brief Build `l >= r`. */
	static tref build_bf_gteq(tref l, tref r);
	/** @brief Build `!(l >= r)`. */
	static tref build_bf_ngteq(tref l, tref r);
	/** @brief Build `l < r`. */
	static tref build_bf_lt(tref l, tref r);
	/** @brief Build `!(l < r)`. */
	static tref build_bf_nlt(tref l, tref r);

	// Term (bf) builders
	/** @brief Build `fall l r`. */
	static tref build_bf_fall(tref l, tref r);
	/** @brief Build `fex l r`. */
	static tref build_bf_fex(tref l, tref r);
	/** @brief Build `l | r`. */
	static tref build_bf_or(tref l, tref r);
	/** @brief Build disjunction of all terms in @p bfs with type @p type_id. */
	static tref build_bf_or(const auto& bfs, size_t type_id);
	/** @brief Build `l nor r` (negated or). */
	static tref build_bf_nor(tref l, tref r);
	/** @brief Build `l ^ r`. */
	static tref build_bf_xor(tref l, tref r);
	/** @brief Build `l xnor r`. */
	static tref build_bf_xnor(tref l, tref r);
	/** @brief Build `l & r`. */
	static tref build_bf_and(tref l, tref r);
	/** @brief Build conjunction of all terms in @p bfs with type @p type_id. */
	static tref build_bf_and(const auto& bfs, size_t type_id);
	/** @brief Build `l nand r`. */
	static tref build_bf_nand(tref l, tref r);
	/** @brief Build `! l`. */
	static tref build_bf_neg(tref l);
	/** @brief Build `l << r` (left shift). */
	static tref build_bf_shl(tref l, tref r);
	/** @brief Build `l >> r` (right shift). */
	static tref build_bf_shr(tref l, tref r);
	/** @brief Build `l + r`. */
	static tref build_bf_add(tref l, tref r);
	/** @brief Build `l - r`. */
	static tref build_bf_sub(tref l, tref r);
	/** @brief Build `l * r`. */
	static tref build_bf_mul(tref l, tref r);
	/** @brief Build `l / r`. */
	static tref build_bf_div(tref l, tref r);
	/** @brief Build `l % r`. */
	static tref build_bf_mod(tref l, tref r);

	// Terminal, variable and constant builders
	/** @brief Build the `T` (true) BA constant for type @p ba_tid. */
	static tref build_bf_t_type(size_t ba_tid);
	/** @brief Build the `F` (false) BA constant for type @p ba_tid. */
	static tref build_bf_f_type(size_t ba_tid);
	/** @brief Build a raw BA-constant node for @p constant with type @p ba_type_id. */
	static tref build_ba_constant(const constant& constant, size_t ba_type_id);
	/** @brief Build a `bf` BA-constant node, optionally with right sibling @p right. */
	static tref build_bf_ba_constant(const constant& constant, size_t ba_type_id, tref right = nullptr);
	/** @brief Build an uninterpreted constant node with names @p name1, @p name2. */
	static tref build_bf_uconst(const std::string& name1, const std::string& name2, size_t type_id);
	/** @brief Build a variable-name node from string id @p sid. */
	static tref build_var_name(size_t sid);
	/** @brief Build a variable-name node from @p name. */
	static tref build_var_name(const std::string& name);
	/** @brief Build a variable-name node with auto-generated name from @p index. */
	static tref build_var_name_indexed(size_t index);
	/** @brief Build a canonised I/O variable from @p name. */
	static tref build_canonized_io_var(const std::string& name);
	/** @brief Build a generic typed variable with type @p type_id. */
	static tref build_variable(size_t type_id);
	/** @brief Build a typed variable with name node @p var_name_node. */
	static tref build_variable(tref var_name_node, size_t type_id);
	/** @brief Build a typed variable from string @p name. */
	static tref build_variable(const std::string& name, size_t type_id);
	/** @brief Build a bf-level typed variable. */
	static tref build_bf_variable(tref var_name_node, size_t type_id);
	/** @brief Build a bf-level typed variable (auto name). */
	static tref build_bf_variable(size_t type_id);
	/** @brief Build a bf-level typed variable from @p name. */
	static tref build_bf_variable(const std::string& name, size_t type_id);
	/** @brief Build an input I/O variable. */
	static tref build_in_var(tref var_name_node, size_t type_id);
	/** @brief Build an input I/O variable with explicit offset node. */
	static tref build_in_var(tref var_name_node, tref offset_node, size_t type_id);
	/** @brief Build an input variable at discrete time position @p pos. */
	static tref build_in_var_at_n(tref var_name_node, int_t pos, size_t type_id);
	/** @brief Build an input variable at position @p pos from @p name. */
	static tref build_in_var_at_n(const std::string& name, int_t pos, size_t type_id);
	/** @brief Build an indexed input variable at position @p pos. */
	static tref build_in_var_at_n_indexed(size_t index, int_t pos, size_t type_id);
	/** @brief Build an input variable at the current time symbol @p t. */
	static tref build_in_var_at_t(tref var_name_node, size_t type_id, std::string t = "t");
	/** @brief Build an indexed input variable at @p t. */
	static tref build_in_var_at_t_indexed(size_t index, size_t type_id, std::string t = "t");
	/** @brief Build an input variable at `t - shift`. */
	static tref build_in_var_at_t_minus(tref var_name_node, size_t shift, size_t type_id, std::string t = "t");
	/** @brief Build an input variable at `t - shift` from @p var_name. */
	static tref build_in_var_at_t_minus(const std::string& var_name, size_t shift, size_t type_id, std::string t = "t");
	/** @brief Build an indexed input variable at `t - shift`. */
	static tref build_in_var_at_t_minus_indexed(size_t index, size_t shift, size_t type_id, std::string t = "t");
	/** @brief Build an output I/O variable. */
	static tref build_out_var(tref var_name_node, size_t type_id);
	/** @brief Build an output I/O variable with explicit offset node. */
	static tref build_out_var(tref var_name_node, tref offset_node, size_t type_id);
	/** @brief Build an output variable at position @p pos. */
	static tref build_out_var_at_n(tref var_name_node, int_t pos, size_t type_id);
	/** @brief Build an output variable at position @p pos from @p name. */
	static tref build_out_var_at_n(const std::string& name, int_t pos, size_t type_id);
	/** @brief Build an indexed output variable at position @p pos. */
	static tref build_out_var_at_n_indexed(size_t index, int_t pos, size_t type_id);
	/** @brief Build an output variable at @p t. */
	static tref build_out_var_at_t(tref var_name_node, size_t type_id, std::string t = "t");
	/** @brief Build an indexed output variable at @p t. */
	static tref build_out_var_at_t_indexed(size_t index, size_t type_id, std::string t = "t");
	/** @brief Build an output variable at `t - shift`. */
	static tref build_out_var_at_t_minus(tref var_name_node, size_t shift, size_t type_id, std::string t = "t");
	/** @brief Build an output variable at `t - shift` from @p io_var_node. */
	static tref build_out_var_at_t_minus(const std::string& io_var_node, size_t shift, size_t type_id, std::string t = "t");
	/** @brief Build an indexed output variable at `t - shift`. */
	static tref build_out_var_at_t_minus_indexed(size_t index, size_t shift, size_t type_id, std::string t = "t");

	/** @brief Build a full `spec` tree from an `rr<node>` recurrence relation. */
	static tref build_spec(const rr<node>& nso_rr);

	// Reference and symbol builders
	/** @brief Build a symbol node from string id @p sid. */
	static tref build_sym(size_t sid);
	/** @brief Build a symbol node from @p sym_name. */
	static tref build_sym(const std::string& sym_name);
	/** @brief Build an offsets node from @p offsets vector. */
	static tref build_offsets(const trefs& offsets);
	/** @brief Build an offsets node from @p offset string. */
	static tref build_offsets(const std::string& offset);
	/** @brief Build an offsets node from @p offsets string vector. */
	static tref build_offsets(const std::vector<std::string>& offsets);
	/** @brief Build a shift node for @p var at @p shift. */
	static tref build_shift(tref var, size_t shift);
	/** @brief Build a shift node for @p var_name at @p shift. */
	static tref build_shift(const std::string& var_name, size_t shift);
	/** @brief Build a `ref_args` node from @p args. */
	static tref build_ref_args(const trefs& args);
	/** @brief Build a `ref_args` node from variable names and type @p type_id. */
	static tref build_ref_args(const std::vector<std::string>& vars, size_t type_id);
	/** @brief Build a recurrence-relation reference `sym(args)`. */
	static tref build_ref(tref sym, const trefs& args);
	/** @brief Build a recurrence-relation reference from @p sym_name. */
	static tref build_ref(const std::string& sym_name, const trefs& args);
	/** @brief Build a full RR reference `sym[offsets](args)`. */
	static tref build_rr_ref(tref sym, const trefs& offsets, const trefs& args);
	/** @brief Build `sym_name[offsets](args)`. */
	static tref build_rr_ref(const std::string& sym_name, const trefs& offsets, const trefs& args);
	/** @brief Build `sym_name[offset](args)` from a single string offset. */
	static tref build_rr_ref(const std::string& sym_name, const std::string offset, const trefs& args);
	/** @brief Build `sym_name[offset](args)` from an integer offset. */
	static tref build_rr_ref(const std::string& sym_name, size_t offset, const trefs& args);
	/** @brief Build `sym[offset - shift](args)`. */
	static tref build_rr_ref(tref sym, tref offset, size_t shift, const trefs& args);
	/** @brief Build `sym_name[offset - shift](args)`. */
	static tref build_rr_ref(const std::string& sym_name, tref offset, size_t shift, const trefs& args);
	/** @brief Build `sym_name[offset_str - shift](args)`. */
	static tref build_rr_ref(const std::string& sym_name, const std::string& offset, size_t shift, const trefs& args);

private:
	using tt = traverser;

};

/** @brief Return a copy of @p term with all BA type annotations stripped. */
template <NodeType node>
tref untype(tref term);

/** @brief Return @p formula with @p that replaced by @p with. */
template <NodeType node>
tref substitute(tref formula, tref that, tref with);

/** @brief Return @p formula with all substitutions in @p changes applied. */
template <NodeType node>
tref substitute(tref formula, const auto& changes);

// ---------------------------------------------------------------------------
// Printer free functions (tau_tree_printers.tmpl.h)
// ---------------------------------------------------------------------------

/** @brief Print BA variant @p v to @p os. */
template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const std::variant<BAs...>& v);

/** @brief Print node @p n to @p os. */
template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const node<BAs...>& n);

/** @brief Pretty-print tree @p t to @p os. */
template <NodeType node>
std::ostream& operator<<(std::ostream& os, const tree<node>& t);

/** @brief Print traverser @p tt to @p os. */
template <NodeType node>
std::ostream& operator<<(std::ostream& os,
				const typename tree<node>::traverser& tt);

/** @brief Print rewriter rule @p r to @p os. */
template <NodeType node>
std::ostream& print(std::ostream& os, const rewriter::rule& r);
/** @brief Print rewriter rules @p rs to @p os. */
template <NodeType node>
std::ostream& print(std::ostream& os, const rewriter::rules& rs);
/** @brief Print recurrence relation @p rr_ to @p os. */
template <NodeType node>
std::ostream& print(std::ostream& os, const rr<node>& rr_);

/** @brief Return a string representation of rewriter rule @p r. */
template <NodeType node>
std::string to_str(const rewriter::rule& r);
/** @brief Return a string representation of rewriter rules @p rs. */
template <NodeType node>
std::string to_str(const rewriter::rules& rs);
/** @brief Return a string representation of recurrence relation @p rr_. */
template <NodeType node>
std::string to_str(const rr<node>& rr_);

/** @brief Dump rewriter rule @p r to @p os. */
template <NodeType node>
std::ostream& dump(std::ostream& os, const rewriter::rule& r);
/** @brief Dump rewriter rules @p rs to @p os. */
template <NodeType node>
std::ostream& dump(std::ostream& os, const rewriter::rules& rs);
/** @brief Dump recurrence relation @p rr_ to @p os. */
template <NodeType node>
std::ostream& dump(std::ostream& os, const rr<node>& rr_);

/** @brief Return a debug-dump string for rewriter rule @p r. */
template <NodeType node>
std::string dump_to_str(const rewriter::rule& r);
/** @brief Return a debug-dump string for rewriter rules @p rs. */
template <NodeType node>
std::string dump_to_str(const rewriter::rules& rs);
/** @brief Return a debug-dump string for recurrence relation @p rr_. */
template <NodeType node>
std::string dump_to_str(const rr<node>& rr_);

// ---------------------------------------------------------------------------
// Query free functions (tau_tree_queries.tmpl.h)
// ---------------------------------------------------------------------------

/** @brief Return `true` if @p n has nonterminal id @p nt. */
template <NodeType node>
bool is(tref n, size_t nt);

/** @brief Return `true` if @p n has nonterminal @p nt (template param). */
template <NodeType node, typename node::type nt>
bool is(tref n);

/** @brief Return a predicate that tests `is<node>(n, nt)`. */
template <NodeType node>
inline std::function<bool(tref)> is(size_t nt);

/** @brief Return `true` if @p n's nonterminal is in @p nts. */
template <NodeType node>
bool is(tref n, std::initializer_list<size_t> nts);

/** @brief Return a predicate that tests `is<node>(n, nts)`. */
template <NodeType node>
inline std::function<bool(tref)> is(std::initializer_list<size_t> nts);

/** @brief Return `true` if the first child of @p n has nonterminal @p nt. */
template <NodeType node>
bool is_child(tref n, size_t nt);

/** @brief Return `true` if the first child of @p n has nonterminal @p nt (template param). */
template <NodeType node, size_t nt>
bool is_child(tref n);

/** @brief Return a predicate that tests `is_child<node>(n, nt)`. */
template <NodeType node>
inline std::function<bool(tref)> is_child(size_t nt);

/** @brief Return `true` if the first child of @p n is any quantifier. */
template <NodeType node>
bool is_child_quantifier(tref n);

/** @brief Return `true` if @p n is a temporal quantifier (`always`, `sometimes`). */
template <NodeType node>
bool is_temporal_quantifier(tref n);

/** @brief Return `true` if the first child of @p n is a temporal quantifier (`always`, `sometimes`). */
template <NodeType node>
bool is_child_temporal_quantifier(tref n);

/** @brief Return `true` if @p n is a BA element (constant or variable). */
template <NodeType node>
bool is_ba_element(tref n);

/** @brief Return `true` if @p n is an uninterpreted constant. */
template <NodeType node>
bool is_uconst(tref n);

/** @brief Return `true` if @p n is an I/O variable (input or output). */
template <NodeType node>
bool is_io_var(tref n);

/** @brief Return `true` if @p n is an input I/O variable. */
template <NodeType node>
bool is_input_var(tref n);

/** @brief Return `true` if @p n is an output I/O variable. */
template <NodeType node>
bool is_output_var(tref n);

/** @brief Return `true` if @p n is a formula variable or capture variable. */
template <NodeType node>
bool is_var_or_capture(tref n);

/** @brief Return a predicate that tests `is_var_or_capture<node>`. */
template <NodeType node>
inline std::function<bool(tref)> is_var_or_capture();

/** @brief Return `true` if @p n is any quantifier (logical or functional). */
template <NodeType node>
bool is_quantifier(tref n);

/** @brief Return `true` if @p n is a functional quantifier (`fall`, `fex`). */
template <NodeType node>
bool is_functional_quantifier(tref n);

/** @brief Return `true` if @p n is a logical or functional quantifier. */
template <NodeType node>
bool is_logical_or_functional_quant(tref n);

/** @brief Return `true` if @p sub_fm appears as a subtree of @p fm. */
template <NodeType node>
bool contains(tref fm, tref sub_fm);

/** @brief Return `true` if @p n is an atomic formula (no connectives). */
template <NodeType node>
bool is_atomic_fm(tref n);

/** @brief Return `true` if @p n is a CLI command node. */
template <NodeType node>
bool is_cli_cmd(tref n);

/** @brief Return `true` if @p n contains a fallback sub-formula. */
template <NodeType node>
bool has_fallback (tref n);

/** @brief Return `true` if @p eq is an equational assignment. */
template <NodeType node>
bool is_equational_assignment(tref eq);

/** @brief Return `true` if @p eq is a boolean operation. */
template <NodeType node>
bool is_boolean_operation(tref op);

/** @brief Return `true` if @p op is a formula (wff). */
template <NodeType node>
bool is_formula(tref op);

// Visiting continuation predicates (for use with `visit`, `find`,...)

/** @brief Return `true` if @p n is a boolean operation or an intermediate bf. */
template <NodeType node>
bool while_is_boolean_operation(tref n);

/** @brief Return `true` if @p n is a formula (wff). */
template <NodeType node>
bool while_is_formula(tref n);

/** @brief Return `true` if @p n is a non-boolean term. */
template <NodeType node>
bool is_non_boolean_term(tref n);

// Fast extractors (not depending in extractors, just direct access to the tree structure)

/** @brief Return the temporally quantified formula of @p n, n otherwise. */
template <NodeType node>
tref get_temporally_quantified_formula(tref n);

} // namespace idni::tau_lang

/** @brief Hash specialization for `node<BAs...>` enabling use in unordered containers. */
template <typename... BAs> requires idni::tau_lang::BAsPack<BAs...>
struct std::hash<idni::tau_lang::node<BAs...>> {
	size_t operator()(const idni::tau_lang::node<BAs...>& n) const noexcept;
};

#include "tau_tree.tmpl.h"

#endif // __IDNI__TAU__TAU_TREE_H__
