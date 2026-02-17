// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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
template <typename T, typename... Ts>
constexpr bool pack_contains = (std::is_same_v<T, Ts> || ...);

template <typename... BAs>
concept BAsPack = sizeof...(BAs) > 0 && pack_contains<cvc5::Term, BAs...>;

template <typename node>
concept NodeType = requires { // Node Type has to provide
	// node type
	typename node::type;
	// type aliases for the packed variant types
	typename node::constant;
	typename node::ba;
	// nt is convertible to size_t
	{ std::declval<node>().nt } -> std::convertible_to<size_t>;
	// term is convertible to bool
	{ std::declval<node>().term } -> std::convertible_to<bool>;
	// ba is convertible to size_t
	{ std::declval<node>().ba_type } -> std::convertible_to<size_t>;
	// data is convertible to size_t
	{ std::declval<node>().data } -> std::convertible_to<size_t>;
};

// -----------------------------------------------------------------------------
// forward declarations

template <NodeType node> struct rr;
template <NodeType node> struct ba_constants;
template <typename... BAs> requires BAsPack<BAs...> struct nso_factory;
template <typename... BAs> requires BAsPack<BAs...> struct tau_ba;
template <NodeType node> struct io_context;
template <NodeType node> struct tau_spec;

// -----------------------------------------------------------------------------
// Tau tree node (tau_tree_node.tmpl.h)
//
// node is templated by a pack of boolean algebras
// This is used for two purposes:
// 1. determine bitsize required for storing BA type ids
// 2. provide type aliases for packed types so they are accessible everywhere
//
// Node stores its type (nt), is_term flag, BA type id, and data in remaining bits
// TODO There is an extension flag (ext) that indicates if the data is stored in the child node
// but this is not implemented yet.
// Currently the flag is always 0 and the data is stored in the node itself.
//
// node also provides alias node::type for tau_parser::nonterminal
// and provides static name(nt) nad name() methods to get type name string
template <typename... BAs>
requires BAsPack<BAs...>
struct node {
	using node_t = node<BAs...>;
	using type = tau_parser::nonterminal;
	// aliases for recreation of the packed variant
	using constant = std::variant<BAs...>;
	using constant_with_type = std::pair<constant, tref>;
	// alias for nso_factory<BAs...>
	using ba = tau_lang::nso_factory<BAs...>;

	using T = size_t; // just to simplify changes or templating it later

	// bit sizes
	static constexpr size_t bits      = std::numeric_limits<T>::digits;
	static constexpr size_t nt_bits   = tau_parser_data::nt_bits;
	static constexpr size_t data_bits = bits - nt_bits - 1;

	// masks and shifts
	static constexpr size_t nt_mask = (size_t(1) << nt_bits) - size_t(1);
	static constexpr size_t data_mask = (size_t(1) << data_bits) - size_t(1);
	static constexpr size_t ext_shift = data_bits;
	static constexpr size_t term_shift = ext_shift + 1; // +1 for ext
	static constexpr size_t nt_shift = term_shift + 1;  // +1 for term

	// node fields
	const T nt   : nt_bits   = 0; // id of the nonterminal (container of the data index value)
	const T term : 1         = 0; // 1 = is term, 0 = is tau (if term == 0 and ba == 1 it is untyped)
	const T ext  : 1         = 0; // 1 = data is in the child node, 0 = data is in the node
	const T data : data_bits = 0; // data or index in a container determined by nt, bf and ba
                                      // if nt == io_var, data == 1 for input or data == 2 for output
	const unsigned short ba_type;
	const size_t hash;

	// generic constructor
	constexpr node(size_t nt = 0, size_t data = 0, size_t is_term = 0,
			size_t ba_type = 0, size_t ext = 0) noexcept;

	node retype(size_t new_nt) const;
	node ba_retype(size_t new_ba) const;
	node replace_data(size_t new_data) const;

	// factory for a ba constant node
	static constexpr node_t ba_constant(size_t v, size_t ba_tid = 0);

	// factory for a node of a given node::type and BA type id,
	// also sets term to 1
	static constexpr node_t ba_typed(type nt, size_t ba_tid = 0);

	// factories for an io_var node
	static constexpr node_t input_variable(size_t ba_tid = 0);
	static constexpr node_t output_variable(size_t ba_tid = 0);

	// returns name of the nonterminal nt
	static const std::string& name(size_t nt);
	// returns name of the node's nonterminal value
	const std::string& name() const;

	// casts size_t data to int_t
	int_t as_int() const;

	// TODO null node? is it required?
	static constexpr node_t nnull();

	// getter and setter for the raw data access of an extension node
	static constexpr node_t extension(T raw_value);
	constexpr T extension() const noexcept;

	// comparison operators
	std::weak_ordering operator<=>(const node& that) const;
	constexpr bool     operator<  (const node& that) const;
	constexpr bool     operator<= (const node& that) const;
	constexpr bool     operator>  (const node& that) const;
	constexpr bool     operator>= (const node& that) const;
	constexpr auto     operator== (const node& that) const;
	constexpr auto     operator!= (const node& that) const;

	// hash function for node
	constexpr size_t hashit() const;
};

struct tau_parser_nonterminal_groups {
	enum nonterminal {
		typeable_symbol = 1000,
		// quantifiers
		// atomic...
	};
};
// -----------------------------------------------------------------------------
// Tau tree (tau_tree.tmpl.h)
//
// basic Tau language structure for storing terms, formulas and language constructs
//
// - extends lcrs_tree<node<BAs...>> with tau_parser_nonterminals
// - is expected to be templated with node<BAs...>
//
// Tree is usually created by static get calls either manually or transformed
// from a tau parse tree
//
// - inherits tree API from lcrs_tree<node>
// - provides traverser (tt) API for simple traversal and extraction
// - provides tau tree builders
// - provides type aliases for further usage:
//     - tau = tree<node<BAs...>>
//     - node = node<BAs...>
//     - parse_tree = tau_parser::tree

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

	// tree direct API
	// ---------------------------------------------------------------------

	// handles
	tref get() const;
	static const tree& get(const std::optional<tref>& id);
	static const tree& get(const tref id);
	static const tree& get(const htref& h);
	static htref geth(tref id);
	static htref geth(const tree& n);

	// creation (transformation) from tau_parser::tree
	static tref get(const tau_parser::tree& t, get_options& options);
	static tref get(const tau_parser::tree& t, get_options&& options);
	// reget a tree in post order
	static tref reget(tref n);

	// get node with children without triggering hooks
	static tref get_raw(const node& v, const tref* ch = nullptr,
					size_t len = 0, tref r = nullptr);

	// get node with updated right sibling
	static tref get(tref n, tref r);

	// get a copy with new value
	tref replace_value(const node& v) const;

	// creation with tref children
	static tref get(const node& v); // leaf
	static tref get(const node& v, tref ch); // with single child
	static tref get(const node& v, tref ch1, tref ch2); // with two children
	static tref get(const node& v, const tref* ch, size_t len, tref r = nullptr);
	static tref get(const node& v, const trefs& ch, tref r = nullptr);
	static tref get(const node& v, const std::initializer_list<tref>& ch, tref r = nullptr);

	// creation with node children
	static tref get(const node& v, const node& ch);
	static tref get(const node& v, const node& ch1, const node& ch2);
	static tref get(const node& v, const node* ch, size_t len, tref r = nullptr);
	static tref get(const node& v, const std::vector<node>& ch, tref r = nullptr);
	static tref get(const node& v, const std::initializer_list<node>& ch, tref r = nullptr);

	// creation of node::type and children
	static tref get(const node::type& nt); // terminal / leaf
	static tref get(const node::type& nt, tref ch); // with single child
	static tref get(const node::type& nt, tref ch1, tref ch2); // with two children
	static tref get(const node::type& nt, const tref* ch, size_t len,
							tref r = nullptr);
	static tref get(const node::type& nt, const trefs& ch,
			tref r = nullptr); // with vector of children
	static tref get(const node::type& nt, // with initializer list of children
			const std::initializer_list<tref>& ch, tref r = nullptr);
	static tref get(const node::type& nt, const std::string& str); // with string
	// typed variants
	static tref get_typed(const node::type& nt, size_t ba_type_id);
	static tref get_typed(const node::type& nt, tref ch, size_t ba_type_id);
	static tref get_typed(const node::type& nt, tref ch1, tref ch2, size_t ba_type_id);
	static tref get_typed(const node::type& nt, const tref* ch, size_t len,
				size_t ba_type_id, tref r = nullptr);
	static tref get_typed(const node::type& nt, const trefs& ch,
				size_t ba_type_id, tref r = nullptr); // with vector of children
	static tref get_typed(const node::type& nt,
				// with initializer list of children
				const std::initializer_list<tref>& ch, size_t ba_type_id, tref r = nullptr);
	static tref get_typed(const node::type& nt, const std::string& str, size_t ba_type_id);

	// terminals
	// creates a num node from a number n
	static tref get_num(size_t n);
	// creates an integer node from an integer i
	static tref get_integer(int_t i);

	// constants
	// creates a ba_constant node from it's value and ba type name
	static tref get_ba_constant(const constant& constant,
				    const tref type_tree);
	// creates a ba_constant node from it's value and ba type id
	static tref get_ba_constant(const constant& constant,
				    size_t ba_type_id);
	// creates a ba_constant node from constant source and type name
	static tref get_ba_constant(const std::string& constant_source,
				    tref type_tree);
	// creates a ba_constant node from constant source dict id and ba type id
	static tref get_ba_constant_from_source(size_t constant_source_sid,
				    size_t ba_type_id);
	// creates a ba_constant node from constant_id and ba type id
	static tref get_ba_constant(size_t constant_id, size_t ba_type_id);
	// creates a ba_constant node from a pair of constant_id and ba_type_id
	static tref get_ba_constant(const std::pair<constant,
		tref>& typed_const);
	// creates a ba_constant node from a pair of constant_id and ba_type_id
	static tref get_ba_constant(
		const std::optional<std::pair<constant, tref>>&
		typed_const);

	// children
	size_t children_size() const;

	bool get_children(tref *ch, size_t& len) const;
	trefs get_children() const;
	tref_range<node> children() const;
	tree_range<tree> children_trees() const;

	tref child(size_t n) const;
	tref first()  const;
	tref second() const;
	tref third()  const;
	tref only_child() const;
	bool has_child() const;

	// fast traverse to a first child
	tref trim() const;
	static tref trim(tref t);
	// fast traverse to a first child of a first child
	tref trim2() const;
	static tref trim2(tref t);

	const tree& operator[](size_t n) const;
	const tree& child_tree(size_t n) const;
	const tree& first_tree()  const;
	const tree& second_tree() const;
	const tree& third_tree()  const;
	const tree& only_child_tree() const;
	const tree& right_sibling_tree() const;

	std::ostream& print(std::ostream& o) const;
	std::ostream& print_tree(std::ostream& o, size_t s = 0) const;
	std::string to_str() const;
	std::string tree_to_str() const;
	std::string dump_to_str(bool subtree = true) const;
	std::ostream& dump(std::ostream& os, bool subtree = true) const;
	const tree& dump(bool subtree = true) const;
	static std::ostream& dump(std::ostream& os, tref n, bool subtree = true);

	// nt category helpers
	static bool is_digital_nt(size_t nt);
	static bool is_string_nt(size_t nt);
	static bool is_term_nt(size_t nt, size_t parent_nt);
	static bool is_term_nt(size_t nt);

	// fast access helpers
	size_t data() const;
	size_t child_data() const;

	bool is(size_t nt) const;
	bool is_string() const;
	bool is_integer() const;
	bool is_num() const;
	bool is_ba_constant() const;
	bool is_term() const;
	bool is_input_variable() const;
	bool is_output_variable() const;

	bool equals_0() const;
	bool equals_1() const;
	bool equals_F() const;
	bool equals_T() const;

	bool child_is(size_t nt) const;

	node::type get_type() const;
	const std::string& get_type_name() const;
	const std::string& get_string() const;
	int_t get_integer() const;
	size_t get_num() const;
	size_t get_ba_constant_id() const;
	constant get_ba_constant() const;
	// TODO (LOW) rename to get_ba_type_id and get_ba_type as in constants
	size_t get_ba_type() const;
	std::string get_ba_type_name() const;
	tref get_ba_type_tree() const;
	const trefs& get_free_vars() const;

	static tref untype(tref term);

	// Substitution method for typed tau trees
	tref substitute(tref that, tref with) const;
	// Substitution method for typed tau trees applying several substitutions
	tref substitute(const auto& changes) const;

	// ---------------------------------------------------------------------
	// helpers for querying

	// TODO (LOW) this could be move to the parser

	std::vector<trefs> select_top_by_predicates(
		const std::function<bool(const tref&)>* queries, const size_t count) const;
	std::vector<trefs> select_top_by_predicates(
		const std::initializer_list<std::function<bool(const tref&)>>& queries) const;
	std::vector<trefs> select_top_by_predicates(
		const std::vector<std::function<bool(const tref&)>>& queries) const;
	std::vector<trefs> select_top_until_by_predicates(
		const std::function<bool(const tref&)>* queries, const size_t count,
		const auto& until) const;
	std::vector<trefs> select_top_until_by_predicates(
		const std::initializer_list<std::function<bool(const tref&)>>& queries,
		const auto& until) const;
	std::vector<trefs> select_top_until_by_predicates(
		const std::vector<std::function<bool(const tref&)>>& queries,
		const auto& until) const;
	std::vector<trefs> select_all_by_predicates(
		const std::function<bool(const tref&)>* queries, const size_t count) const;
	std::vector<trefs> select_all_by_predicates(
		const std::initializer_list<std::function<bool(const tref&)>>& queries) const;
	std::vector<trefs> select_all_by_predicates(
		const std::vector<std::function<bool(const tref&)>>& queries) const;
	std::vector<trefs> select_all_until_by_predicates(
		const std::function<bool(const tref&)>* queries, const size_t count,
		const auto& until) const;
	std::vector<trefs> select_all_until_by_predicates(
		const std::initializer_list<std::function<bool(const tref&)>>& queries,
		const auto& until) const;
	std::vector<trefs> select_all_until_by_predicates(
		const std::vector<std::function<bool(const tref&)>>& queries,
		const auto& until) const;

	// ---------------------------------------------------------------------
	// from parser (tau_tree_from_parser.tmpl.h)

	struct get_options {
		tau_parser::parse_options parse{};
		std::map<std::string, tref> named_constants{};
		bool infer_ba_types = true;
		bool reget_with_hooks = true;
		const std::vector<htref>* definition_heads = nullptr;
		subtree_map<node, size_t>* global_scope = nullptr;
		io_context<node>* context = nullptr;
	};

	// creation from parser result or parser input (string, stream, file)
	static tref get(tau_parser::result& result, get_options& options);
	static tref get(tau_parser::result& result, get_options&& options);
	static tref get(const std::string& str);
	static tref get(const std::string& str, get_options& options);
	static tref get(const std::string& str, get_options&& options);
	static tref get(std::istream& is, get_options& options);
	static tref get(std::istream& is, get_options&& options);
	static tref get_from_file(const std::string& filename,
						get_options& options);
	static tref get_from_file(const std::string& filename,
						get_options&& options);

	// ---------------------------------------------------------------------
	// tree::traverser / tt API (tau_tree_traverser.tmpl.h)

	struct traverser;

	// extractor used on a traverser node to extract value of a result_type
	template <typename result_type>
	struct extractor {
		using function = std::function<result_type(const traverser&)>;
		extractor(const function& e);
		result_type operator()(const traverser& t) const;
	private:
		function e;
	};

	// tree wrapper for simple traversing using | and || operators
	// and | extractor<result_type>
	struct traverser {
		using type = typename node::type;
		traverser();
		traverser(tref r);
		traverser(const tree& r);
		traverser(const std::optional<tref>& r);
		traverser(const htref& r);
		traverser(const trefs& n);
		bool has_value() const;
		explicit operator bool() const;
		tref value() const;
		const tree& value_tree() const;
		const tree& operator[](size_t n) const;
		trefs values() const;
		std::vector<traverser> traversers() const;
		std::vector<traverser> operator()() const;
		bool is(size_t nt) const;
		bool empty() const;
		size_t size() const;

		// handles
		static const extractor<tref>                ref;
		static const extractor<trefs>               refs;
		static const extractor<htref>               handle;
		static const extractor<const tree<node>&>   Tree;
		// print/dump
		static const extractor<traverser>           dump;
		static const extractor<traverser>           print_tree;
		// node type
		static const extractor<type>                nt;
		// node data
		static const extractor<std::string>         string;
		static const extractor<int_t>               integer;
		static const extractor<size_t>              num;
		static const extractor<size_t>              data;
		static const extractor<size_t>              ba_constant_id;
		static const extractor<size_t>              ba_type;
		static const extractor<constant>            ba_constant;
		static const extractor<size_t>              bv_constant_id;
		static const extractor<constant>            bv_constant;
		static const extractor<size_t>              bv_size;
		// children
		static const extractor<traverser>           only_child;
		static const extractor<traverser>           first;
		static const extractor<traverser>           second;
		static const extractor<traverser>           third;
		static const extractor<traverser>           fourth;
		static const extractor<traverser>           children;
		static const extractor<tref_range<node>>    children_range;
		static const
			extractor<tree_range<tree<node>>>   children_trees_range;
		// (tref) -> tref function tt:f wrapper
		static const extractor<traverser> f(const auto& fn);

		// traverses to the first child of the type nt
		traverser operator|(size_t nt) const;
		// traverses to all children of the type nt
		traverser operator||(size_t nt) const;
		// extract or process values in traverser
		template <typename result_type>
		result_type operator|(const extractor<result_type>&) const;
		// extract or process values in traverser (works equally to operator|)
		template <typename result_type>
		result_type operator||(const extractor<result_type>&) const;
	private:
		void set_values(const trefs& n);
		bool has_value_ = true;
		trefs values_{};
	};
	// traverser operator|(size_t nt) const;
	// traverser operator||(size_t nt) const;

	// wraps this node by a traverser
	traverser operator()() const;

	// ---------------------------------------------------------------------
	// builder (tau_tree_builders.tmpl.h)

	static tref apply_builder(const rewriter::builder& b, trefs n);

	static tref _0(size_t type_id);
	static tref _1(size_t type_id);
	static tref _F();
	static tref _T();
	static tref _0_trimmed(size_t type_id);
	static tref _1_trimmed(size_t type_id);
	static tref _F_trimmed();
	static tref _T_trimmed();
	static const tree<node>& get_0(size_t type_id);
	static const tree<node>& get_1(size_t type_id);
	static const tree<node>& get_F();
	static const tree<node>& get_T();
	static const tree<node>& get_0_trimmed(size_t type_id);
	static const tree<node>& get_1_trimmed(size_t type_id);
	static const tree<node>& get_F_trimmed();
	static const tree<node>& get_T_trimmed();

	// TODO (LOW) this could be somehow easily generatable by parser_gen
	// or maybe create simple builder api, maybe with >> operator
	// formula builders
	static tref build_wff_sometimes(tref n);
	static tref build_wff_always(tref n);
	static tref build_wff_conditional(tref x, tref y, tref z);
	static tref build_wff_all(tref bound_var, tref subformula,
		bool calculate_quant_id = true);
	static tref build_wff_all_many(const trefs& bound_vars, tref subformula);
	static tref build_wff_ex(tref bound_var, tref subformula,
		bool calculate_quant_id = true);
	static tref build_wff_ex_many(const trefs& bound_vars, tref subformula);
	static tref build_wff_imply(tref l, tref r);
	static tref build_wff_rimply(tref l, tref r);
	static tref build_wff_equiv(tref l, tref r);
	static tref build_wff_or(tref l, tref r);
	static tref build_wff_or(const auto& wffs);
	static tref build_wff_xor(tref l, tref r);
	static tref build_wff_and(tref l, tref r);
	static tref build_wff_and(const auto& wffs);
	static tref build_wff_neg(tref n);

	// constraint builders
	static tref build_wff_ctn_neq(tref ctnvar, tref num);
	static tref build_wff_ctn_eq(tref ctnvar, tref num);
	static tref build_wff_ctn_gteq(tref ctnvar, tref num);
	static tref build_wff_ctn_gt(tref ctnvar, tref num);
	static tref build_wff_ctn_lteq(tref ctnvar, tref num);
	static tref build_wff_ctn_lt(tref ctnvar, tref num);

	// wff relational operators of terms
	static tref build_bf_interval(tref x, tref y, tref z);
	static tref build_bf_eq(tref l, tref r);
	static tref build_bf_eq_0(tref l);
	static tref build_bf_neq(tref l, tref r);
	static tref build_bf_neq_0(tref l);
	static tref build_bf_lteq(tref l, tref r);
	static tref build_bf_nlteq(tref l, tref r);
	static tref build_bf_gt(tref l, tref r);
	static tref build_bf_ngt(tref l, tref r);
	static tref build_bf_gteq(tref l, tref r);
	static tref build_bf_ngteq(tref l, tref r);
	static tref build_bf_lt(tref l, tref r);
	static tref build_bf_nlt(tref l, tref r);

	// term builders
	static tref build_bf_fall(tref l, tref r);
	static tref build_bf_fex(tref l, tref r);
	static tref build_bf_or(tref l, tref r);
	static tref build_bf_or(const auto& bfs, size_t type_id);
	static tref build_bf_nor(tref l, tref r);
	static tref build_bf_xor(tref l, tref r);
	static tref build_bf_xnor(tref l, tref r);
	static tref build_bf_and(tref l, tref r);
	static tref build_bf_and(const auto& bfs, size_t type_id);
	static tref build_bf_nand(tref l, tref r);
	static tref build_bf_neg(tref l);
	static tref build_bf_shl(tref l, tref r);
	static tref build_bf_shr(tref l, tref r);
	static tref build_bf_add(tref l, tref r);
	static tref build_bf_sub(tref l, tref r);
	static tref build_bf_mul(tref l, tref r);
	static tref build_bf_div(tref l, tref r);
	static tref build_bf_mod(tref l, tref r);

	// terminals, variables and constants
	static tref build_bf_t_type(size_t ba_tid);
	static tref build_bf_f_type(size_t ba_tid);
	static tref build_ba_constant(const constant& constant,
				      size_t ba_type_id);
	static tref build_bf_ba_constant(const constant& constant,
					 size_t ba_type_id, tref right = nullptr);
	static tref build_bf_uconst(
		const std::string& name1, const std::string& name2, size_t type_id);
	static tref build_var_name(size_t sid);
	static tref build_var_name(const std::string& name);
	static tref build_var_name_indexed(size_t index);
	static tref build_canonized_io_var(const std::string& name);
	static tref build_variable(tref var_name_node, size_t type_id);
	static tref build_variable(const std::string& name, size_t type_id);
	static tref build_bf_variable(tref var_name_node, size_t type_id);
	static tref build_bf_variable(const std::string& name, size_t type_id);
	static tref build_in_var(tref var_name_node, size_t type_id);
	static tref build_in_var(tref var_name_node, tref offset_node, size_t type_id);
	static tref build_in_var_at_n(tref var_name_node, int_t pos, size_t type_id);
	static tref build_in_var_at_n(const std::string& name, int_t pos, size_t type_id);
	static tref build_in_var_at_n_indexed(size_t index, int_t pos, size_t type_id);
	static tref build_in_var_at_t(tref var_name_node, size_t type_id, std::string t = "t");
	static tref build_in_var_at_t_indexed(size_t index, size_t type_id, std::string t = "t");
	static tref build_in_var_at_t_minus(
		tref var_name_node, size_t shift, size_t type_id, std::string t = "t");
	static tref build_in_var_at_t_minus(
		const std::string& var_name, size_t shift, size_t type_id, std::string t = "t");
	static tref build_in_var_at_t_minus_indexed(
		size_t index, size_t shift, size_t type_id, std::string t = "t");
	static tref build_out_var(tref var_name_node, size_t type_id);
	static tref build_out_var(tref var_name_node, tref offset_node, size_t type_id);
	static tref build_out_var_at_n(tref var_name_node, int_t pos, size_t type_id);
	static tref build_out_var_at_n(const std::string& name, int_t pos, size_t type_id);
	static tref build_out_var_at_n_indexed(size_t index, int_t pos, size_t type_id);
	static tref build_out_var_at_t(tref var_name_node, size_t type_id, std::string t = "t");
	static tref build_out_var_at_t_indexed(size_t index, size_t type_id, std::string t="t");
	static tref build_out_var_at_t_minus(
		tref var_name_node, size_t shift, size_t type_id, std::string t = "t");
	static tref build_out_var_at_t_minus(
		const std::string& io_var_node, size_t shift, size_t type_id, std::string t = "t");
	static tref build_out_var_at_t_minus_indexed(
		size_t index, size_t shift, size_t type_id, std::string t = "t");

	static tref build_spec(const rr<node>& nso_rr);

private:
	using tt = traverser;

};

template <NodeType node>
tref untype(tref term);

// Substitution method for typed tau trees
template <NodeType node>
tref substitute(tref formula, tref that, tref with);

// Substitution method for typed tau trees applying several changes
template <NodeType node>
tref substitute(tref formula, const auto& changes);

// -----------------------------------------------------------------------------
// printers (tau_tree_printers.tmpl.h)

template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const std::variant<BAs...>& v);
template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const node<BAs...>& n);
template <NodeType node>
std::ostream& operator<<(std::ostream& os, const tree<node>& t);
template <NodeType node>
std::ostream& operator<<(std::ostream& os,
				const typename tree<node>::traverser& tt);
template <NodeType node>
std::ostream& print(std::ostream& os, const rewriter::rule& r);
template <NodeType node>
std::ostream& print(std::ostream& os, const rewriter::rules& rs);
template <NodeType node>
std::ostream& print(std::ostream& os, const rr<node>& rr_);

template <NodeType node>
std::string to_str(const rewriter::rule& r);
template <NodeType node>
std::string to_str(const rewriter::rules& rs);
template <NodeType node>
std::string to_str(const rr<node>& rr_);

template <NodeType node>
std::ostream& dump(std::ostream& os, const rewriter::rule& r);
template <NodeType node>
std::ostream& dump(std::ostream& os, const rewriter::rules& rs);
template <NodeType node>
std::ostream& dump(std::ostream& os, const rr<node>& rr_);

template <NodeType node>
std::string dump_to_str(const rewriter::rule& r);
template <NodeType node>
std::string dump_to_str(const rewriter::rules& rs);
template <NodeType node>
std::string dump_to_str(const rr<node>& rr_);

// -----------------------------------------------------------------------------
// queries (tau_tree_queries.tmpl.h)

template <NodeType node>
bool is(tref n, size_t nt);

template <NodeType node, typename node::type nt>
bool is(tref n);

// factory method for is predicate
template <NodeType node>
inline std::function<bool(tref)> is(size_t nt);

template <NodeType node>
bool is(tref n, std::initializer_list<size_t> nts);

// factory method for is predicate
template <NodeType node>
inline std::function<bool(tref)> is(std::initializer_list<size_t> nts);

template <NodeType node>
bool is_child(tref n, size_t nt);

template <NodeType node, size_t nt>
bool is_child(tref n);

// factory method for is_child predicate
template <NodeType node>
inline std::function<bool(tref)> is_child(size_t nt);

template <NodeType node>
bool is_child_quantifier(tref n);

template <NodeType node>
bool is_temporal_quantifier(tref n);

template <NodeType node>
bool is_ba_element(tref n);

template <NodeType node>
bool is_uconst(tref n);

template <NodeType node>
bool is_io_var(tref n);

template <NodeType node>
bool is_input_var(tref n);

template <NodeType node>
bool is_output_var(tref n);

template <NodeType node>
bool is_var_or_capture(tref n);

template <NodeType node>
inline std::function<bool(tref)> is_var_or_capture();

template <NodeType node>
bool is_quantifier(tref n);

template <NodeType node>
bool is_functional_quantifier(tref n);

template <NodeType node>
bool is_logical_or_functional_quant(tref n);

template <NodeType node>
bool contains(tref fm, tref sub_fm);

template <NodeType node>
bool is_atomic_fm(tref n);

template <NodeType node>
bool is_cli_cmd(tref n);

template <NodeType node>
bool has_fallback (tref n);

template <NodeType node>
bool is_equational_assignment(tref eq);

} // namespace idni::tau_lang

// Hash for tau node
template <typename... BAs> requires idni::tau_lang::BAsPack<BAs...>
struct std::hash<idni::tau_lang::node<BAs...>> {
	size_t operator()(const idni::tau_lang::node<BAs...>& n) const noexcept;
};

#include "tau_tree.tmpl.h"

#endif // __IDNI__TAU__TAU_TREE_H__