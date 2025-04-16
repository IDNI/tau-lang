// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TAU_TREE_H__
#define __IDNI__TAU__TAU_TREE_H__

#include <initializer_list>
#include <variant>
#include <numeric>

#include "dict.h"
#include "tau_parser.generated.h"
#include "rr.h"

namespace idni::tau_lang {

// helper to get size of boolean algebras variant pack
template <typename... BAs>
constexpr size_t BAs_size = std::variant_size<std::variant<BAs...>>::value;

// helper to get bitsize required for storing BA type ids
template <typename... BAs>
constexpr size_t BAs_bitsize = BAs_size<BAs...> <= 1 ? 1 : 
		//static_cast<size_t>(std::ceil(std::log2(BAs_size<BAs...>)));
                (sizeof(size_t) * 8 - __builtin_clzl(BAs_size<BAs...> - 1));

template <typename... BAs>
struct ba_constants;

// -----------------------------------------------------------------------------
// Tau tree node
//
// node is templated by a pack of boolean algebras
// This is used in the node for two purposes:
// 1. determine bitsize required for storing BA type ids
// 2. provide type aliases for packed types so they are accessible everywhere
//
// Node stores its type (nt), is_term flag, BA type id, and data in remaining bits
// TODO There is an extension flag (ext) that indicates if the data is stored in the child node
// but this is not implemented yet.
// Currently the flag is always 0 and the data is stored in the node itself.
//
template <typename... BAs>
struct node {
	using node_t = node<BAs...>;
	using type = tau_parser::nonterminal;

	// alias for recreation of the packed variant
	using bas_variant = std::variant<BAs...>;
	// alias for ba_constants<BAs...> pool and binder
	using ba_constants_t = ba_constants<BAs...>;

	using T = size_t; // just to simplify changes or templating it later;

	static constexpr size_t bits      = std::numeric_limits<T>::digits;
	static constexpr size_t nt_bits   = tau_parser_data::nt_bits;
	static constexpr size_t ba_bits   = BAs_bitsize<BAs...>;
	static constexpr size_t data_bits = bits - nt_bits - 1 - ba_bits - 1;
	
	// masks and shifts
	static constexpr size_t nt_mask = (size_t(1) << nt_bits) - size_t(1);
	static constexpr size_t data_mask = (size_t(1) << data_bits) - size_t(1);
	static constexpr size_t ext_shift = data_bits;
	static constexpr size_t ba_shift = ext_shift + 1; // +1 for ext
	static constexpr size_t term_shift = ba_shift + ba_bits;
	static constexpr size_t nt_shift = term_shift + 1;  // +1 for term
	static constexpr size_t ba_mask = ((size_t(1) << ba_bits) - size_t(1)) << ba_shift;

	const T nt   : nt_bits   = 0; // id of the nonterminal (container of the data index value)
	const T term : 1         = 0; // 1 = is term, 0 = is tau (if term == 0 and ba == 1 it is untyped)
	const T ba   : ba_bits   = 0; // id of the ba type, 0 = untyped
	const T ext  : 1         = 0; // 1 = data is in the child node, 0 = data is in the node
	const T data : data_bits = 0; // data or index in a container determined by nt, bf and ba

	const size_t hash;

	// generic constructor
	constexpr node(size_t nt = 0, size_t data = 0, size_t is_term = 0,
			size_t ba_type = 0, size_t ext = 0) noexcept;

	// factory for a ba constant node
	static constexpr node_t ba_constant(size_t v, size_t ba_tid = 0);

	// casts size_t data to int_t
	int_t as_int() const;

	// TODO null node? is it required? 
	static constexpr node_t nnull();

	// getter and setter for the raw data access of an extension node 
	static constexpr node_t extension(T raw_value);
	constexpr T extension() const noexcept;

	// comparison operators
	auto operator<=>(const node& that) const;
	constexpr bool operator<(const node& that) const;
	constexpr bool operator<=(const node& that) const;
	constexpr bool operator>(const node& that) const;
	constexpr bool operator>=(const node& that) const;
	constexpr auto operator==(const node& that) const;
	constexpr auto operator!=(const node& that) const;
	// hash function
	constexpr size_t hashit() const;
};

template <typename... BAs>	
std::ostream& operator<<(std::ostream& os, const node<BAs...>& n);

template <typename... BAs>
struct ba_constants;

// -----------------------------------------------------------------------------
// Tau tree
//
// basic Tau language structure for storing terms, formulas and language constructs
//
// - extends lcrs_tree<node<BAs...>> with tau_parser_nonterminals
// - is expected to be templated with node<BAs...>
// 
// Tree is usually created by static get calls either manually or transformed
// from a tau parse tree
//
// - inherits tree API from lcrs_tree<N>
// - provides traverser (tt) API for simple traversal and extraction
// - provides tau tree builders
// - provides type aliases for further usage:
//     - tau = tree<node<BAs...>>
//     - node = node<BAs...>
//     - parse_tree = tau_parser::tree
//     - parse_options = tau_parser::parse_options
//

template <typename N>
struct tree : public idni::lcrs_tree<N>, public tau_parser_nonterminals {
	using base_t = idni::lcrs_tree<N>;
	using tau = tree<N>;
	using node = N;
	using parse_tree = tau_parser::tree;
	using parse_options = tau_parser::parse_options;
	using bas_variant = typename node::bas_variant;
	using ba_constants_t = typename node::ba_constants_t;

	// tree direct API
	// ---------------------------------------------------------------------

	// handles
	tref get() const;
	static const tree& get(const tref id);
	static const tree& get(const htree::sp& h);
	static htree::sp geth(tref id);

	// creation (transformation) from tau_parser::tree
	template <typename binder>
	static tref get(binder& bind, const tau_parser::tree& t);
	static tref get(const tau_parser::tree& t);

	// creation with tref children
	static tref get(const node& v); // leaf
	static tref get(const node& v, tref ch); // with single child
	static tref get(const node& v, tref ch1, tref ch2); // with two children
	static tref get(const node& v, const tref* ch, size_t len);
	static tref get(const node& v, const trefs& ch);
	static tref get(const node& v, const std::initializer_list<tref>& ch);

	// creation with node children
	static tref get(const node& v, const node& ch);
	static tref get(const node& v, const node& ch1, const node& ch2);
	static tref get(const node& v, const node* ch, size_t len);
	static tref get(const node& v, const std::vector<node>& ch);
	static tref get(const node& v, const std::initializer_list<node>& ch);

	// creation of node::type and children
	static tref get(const node::type& nt); // terminal / leaf
	static tref get(const node::type& nt, tref ch); // with single child
	static tref get(const node::type& nt, tref ch1, tref ch2); // with two children
	static tref get(const node::type& nt, const tref* ch, size_t len); // with array of children
	static tref get(const node::type& nt, const trefs& ch); // with vector of children
	static tref get(const node::type& nt, // with initializer list of children
					const std::initializer_list<tref>& ch);
	static tref get(const node::type& nt, const std::string& str); // with string

	// creation from parser result or parser input (string, stream, file)
	static tref get(tau_parser::result& result);
	static tref get(const std::string& str, parse_options options = {});
	static tref get(std::istream& is, parse_options options = {});
	static tref get_from_file(const std::string& filename,
						parse_options options = {});

	// with binder
	template <typename binder>
	static tref get(binder& bind, tau_parser::result& result);
	template <typename binder>
	static tref get(binder& bind, const std::string& str,
						parse_options options = {});
	template <typename binder>
	static tref get(binder& bind, std::istream& is,
						parse_options options = {});
	template <typename binder>
	static tref get_from_file(binder& bind, const std::string& filename,
						parse_options options = {});

	// ---------------------------------------------------------------------
	// terminals

	static tref get_num(size_t v);
	static tref get_integer(int_t v);
	static tref get_ba_constant(size_t v);
	static tref get_ba_constant(size_t v, size_t type);

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

	tref trim() const { return first(); }
	static tref trim(tref t) { return get(t).first(); }

	const tree& operator[](size_t n) const;
	const tree& child_tree(size_t n) const;
	const tree& first_tree()  const;
	const tree& second_tree() const;
	const tree& third_tree()  const;
	const tree& only_child_tree() const;
	
	std::ostream& print(std::ostream& o) const;
	std::ostream& print_tree(std::ostream& o, size_t s = 0) const;

	// nt category helpers
	static bool is_digital_nt(size_t nt);
	static bool is_string_nt(size_t nt);

	// fast access helpers
	size_t data() const;
	size_t child_data() const;

	bool is(size_t nt) const;
	bool is_string() const;
	bool is_integer() const;
	bool is_num() const;
	bool is_ba_constant() const;

	bool child_is(size_t nt) const;

	node::type get_type() const;
	const std::string& get_type_name() const;
	const std::string& get_string() const;
	int_t get_integer() const;
	size_t get_num() const;
	size_t get_ba_constant_id() const;
	bas_variant get_ba_constant() const;

	// ---------------------------------------------------------------------
	// various extractors

	static void get_leaves(tref n, node::type branch, trefs& leaves);
	static trefs get_leaves(tref n, node::type branch);
	static trefs get_dnf_wff_clauses(tref n);
	static trefs get_cnf_wff_clauses(tref n);
	static trefs get_dnf_bf_clauses(tref n);
	static trefs get_cnf_bf_clauses(tref n);

	static rewriter::rules get_rules(tref r);
	template <typename binder>
	static rewriter::library get_library(binder& bind,
						const std::string& source);
	static rewriter::library get_library(const std::string& source);
	template <typename binder>	
	static rewriter::builder get_builder(binder& bind,
						const std::string& source);
	static rewriter::builder get_builder(const std::string& source);
	static rewriter::builder get_builder(tref ref);

	static rewriter::rules get_rec_relations(tref r);

	static std::optional<rr> get_nso_rr(tref ref);

	// tree::traverser / tt API
	// ---------------------------------------------------------------------

	struct traverser;

	// extractor used on a traverser node to extract value of a result_type
	template <typename result_type>
	struct extractor {
		using function = std::function<result_type(const traverser&)>;
		extractor(const function& e) : e(e) {}
		result_type operator()(const traverser& t) const
						{ return e(t); }
	private:
		function e;
	};

	// tree wrapper for simple traversing using | and || operators
	// and | extractor<result_type>
	//                only_child, opt_nonterminal, nonterminal,
	//                terminals, first, second, third)
	struct traverser {
		traverser();
		traverser(tref r);
		traverser(const htree::sp& r);
		traverser(const trefs& n);
		bool has_value() const;
		explicit operator bool() const;
		tref value() const;
		const tree& value_tree() const;
		const tree& operator[](size_t n) const;
		const trefs& values() const;
		std::vector<traverser> traversers() const;
		std::vector<traverser> operator()() const;
		bool is(size_t nt) const { return value_tree().is(nt); }
		bool empty() const;
		size_t size() const;

		// template <node::type NT>
		// static inline const extractor<bool> is{
		// 	[](const traverser& t) {
		// 		if (!t) return false;
		// 		return t.value_tree().get_type() == NT;
		// 	}};
		static inline const extractor<tref> ref{
			[](const traverser& t) -> tref { return t.value(); }};
		static inline const extractor<traverser> children{
			[](const traverser& t) {
				if (!t) return traverser();
				return traverser(t.value_tree()
						.get_children());
			}};
		static inline const extractor<tref_range<node>>
							children_range{
			[](const traverser& t) {
				if (!t) return traverser();
				return traverser(t.value_tree()
						.children());
			}};
		static inline const extractor<tree_range<tree>>
							children_trees_range{
			[](const traverser& t) {
				if (!t) return traverser();
				return traverser(t.value_tree()
						.children_trees());
			}};
		static inline const extractor<traverser> only_child{
			[](const traverser& t) -> traverser {
				if (!t) return {};
				tref r = t.value_tree().only_child();
				if (!r) return {};
				return traverser(r);
			}};
		static inline const extractor<traverser> first{
			[](const traverser& t) {
				if (!t) return traverser();
				tref r = t.value_tree().first();
				if (!r) return traverser();
				return traverser(r);
			}};
		static inline const extractor<traverser> second{
			[](const traverser& t) {
				if (!t) return traverser();
				tref r = t.value_tree().second();
				if (!r) return traverser();
				return traverser(r);
			}};
		static inline const extractor<traverser> third{
			[](const traverser& t) {
				if (!t) return traverser();
				tref r = t.value_tree().third();
				if (!r) return traverser();
				return traverser(r);
			}};
		static inline const extractor<std::string> string{
			[](const traverser& t) {
				if (!t) return std::string();
				return t.value_tree().get_string();
			}};
		static inline const extractor<typename node::type> nt {
			[](const traverser& t) {
				if (!t) return
					static_cast<typename node::type>(0);
				return t.value_tree().get_type();
			}};
		static inline const extractor<int_t> integer{
			[](const traverser& t) {
				if (!t) return 0;
				return t.value_tree().get_integer();
			}};
		static inline const extractor<size_t> num{
			[](const traverser& t) -> size_t {
				if (!t) return 0;
				return t.value_tree().get_num();
			}};
		static inline const extractor<size_t> data{
			[](const traverser& t) -> size_t {
				if (!t) return 0;
				return t.value_tree().data();
			}};
		static inline const extractor<size_t> ba_constant_id{
			[](const traverser& t) -> size_t {
				if (!t) return 0;
				return t.value_tree().get_ba_constant_id();
			}};
		static inline const extractor<bas_variant> ba_constant
		{
			[](const traverser& t) -> bas_variant {
				return ba_constants_t::get(
					t.value_tree().get_ba_constant_id());
			}
		};
		static inline const extractor<traverser> dump{
			[](const traverser& t) {
				if (!t) return t;
				t.value_tree().dump(std::cout) << "\n";
				return t;
			}};
		static inline const extractor<traverser> print_tree{
			[](const traverser& t) {
				if (!t) return t;
				t.value_tree().print_tree(std::cout) << "\n";
				return t;
			}};
		static inline const extractor<htree::sp> handle{
			[](const traverser& t) {
				if (!t) return htree::sp();
				return geth(t.value());
			}};

		traverser operator|(size_t nt) const;
		traverser operator||(size_t nt) const;
		template <typename result_type>
		result_type operator|(const extractor<result_type>&)
								const;
		template <typename result_type>
		result_type operator||(const extractor<result_type>&)
								const;
	private:
		void set_values(const trefs& n);
		bool has_value_ = true;
		trefs values_{};
	};
	// traverser operator|(size_t nt) const;
	// traverser operator||(size_t nt) const;

	using tt = traverser;

	// ---------------------------------------------------------------------
	// builder

	static tref apply_builder(const rewriter::builder& b, trefs n);

	static rewriter::builder bldr_wff_eq;
	static rewriter::builder bldr_bf_splitter;
	static rewriter::builder bldr_bf_not_less_equal;
	static rewriter::builder bldr_bf_interval;
	static rewriter::builder bldr_bf_nleq_upper;
	static rewriter::builder bldr_bf_nleq_lower;

	static const tree<N>& _0();
	static const tree<N>& _0_trimmed();
	static const tree<N>& _1();
	static const tree<N>& _1_trimmed();
	static const tree<N>& _F();
	static const tree<N>& _F_trimmed();
	static const tree<N>& _T();
	static const tree<N>& _T_trimmed();

	// TODO (LOW) this could be somehow easily generatable by parser_gen
	// or maybe create simple builder api, maybe with >> operator
	static tref build_variable(const std::string& name);
	static tref build_in_var_name(size_t index);
	static tref build_type(const std::string& type);
	static tref build_bf_t_type(const std::string& type);
	static tref build_bf_t_type(tref type);
	static tref build_bf_f_type(const std::string& type);
	static tref build_bf_f_type(tref type);
	static tref build_in_variable_at_n(tref in_var_name, size_t num);
	static tref build_in_variable_at_n(size_t index, size_t num);
	static tref build_in_variable_at_n(const std::string& name, int_t pos);
	static tref build_in_variable_at_t(tref in_var_name);
	static tref build_in_variable_at_t(size_t index);
	static tref build_in_variable_at_t_minus(const std::string& var_name,
								int_t shift);
	static tref build_in_variable_at_t_minus(tref in_var_name, size_t num);
	static tref build_in_variable_at_t_minus(size_t index, size_t num);
	static tref build_out_var_name(size_t index);
	static tref build_out_var_name(const std::string& name);
	static tref build_out_variable_at_t(tref out_var_name);
	static tref build_out_variable_at_t(size_t index);
	static tref build_out_variable_at_n(tref out_var_name, size_t num);
	static tref build_out_variable_at_n(size_t index, size_t num);
	static tref build_out_variable_at_n(const std::string& name, int_t pos);
	static tref build_out_variable_at_t_minus(
				const std::string& out_var_name, int_t shift);
	static tref build_out_variable_at_t_minus(tref out_var_name, size_t num);
	static tref build_out_variable_at_t_minus(size_t index, size_t num);
	// static tref bf_variable(const std::string& name);
	static tref build_bf_constant(tref cte);
	static tref build_bf_constant(tref cte, tref type);
	static tref build_bf_constant(tref cte, const std::string& type);
	// build_bf_and_constant and build_bf_or_constant
	static tref build_bf_uniter_const(const std::string& name1,
						const std::string& name2);
	static tref build_wff_eq(tref l, tref r);
	static tref build_wff_eq(tref l);
	static tref build_wff_neq(tref l);
	static tref build_wff_and(tref l, tref r);
	static tref build_wff_and(const auto& wffs);
	static tref build_wff_or(tref l, tref r);
	static tref build_wff_or(const auto& wffs);
	static tref build_wff_neg(tref n);
	static tref build_wff_xor_from_def(tref l, tref r);
	static tref build_wff_xor(tref l, tref r);
	static tref build_wff_imply(tref l, tref r);
	static tref build_wff_conditional(tref x, tref y, tref z);
	static tref build_wff_equiv(tref l, tref r);
	static tref build_wff_all(tref l, tref r);
	static tref build_wff_ex(tref l, tref r);
	static tref build_wff_sometimes(tref n);
	static tref build_wff_always(tref n);
	static tref build_bf_and(tref l, tref r);
	static tref build_bf_and(const auto& bfs);
	static tref build_bf_or(tref l, tref r);
	static tref build_bf_or(const auto& bfs);
	static tref build_bf_neg(tref n);
	static tref build_bf_xor_from_def(tref l, tref r);
	static tref build_bf_xor(tref l, tref r);
	static tref build_bf_less(tref l, tref r);
	static tref build_bf_nless(tref l, tref r);
	static tref build_bf_less_equal(tref l, tref r);
	static tref build_bf_nleq(tref l, tref r);
	static tref build_bf_interval(tref x, tref y, tref z);
	static tref build_bf_nleq_lower(tref l, tref r);
	static tref build_bf_nleq_upper(tref l, tref r);
	static tref build_bf_greater(tref l, tref r);
	static tref build_bf_ngreater(tref l, tref r);
	static tref build_bf_greater_equal(tref l, tref r);
	static tref build_bf_ngeq(tref l, tref r);
	static tref build_wff_ctn_greater_equal(tref ctnvar, tref num);
	static tref build_wff_ctn_greater(tref ctnvar, tref num);
	static tref build_wff_ctn_less_equal(tref ctnvar, tref num);
	static tref build_wff_ctn_less(tref ctnvar, tref num);
	static tref build_wff_ctn_eq(tref ctnvar, tref num);
	static tref build_wff_ctn_neq(tref ctnvar, tref num);

};

// -----------------------------------------------------------------------------
// tau and tt printers

template <typename node>
std::ostream& operator<<(std::ostream& os, const tree<node>& t);

template <typename node>
std::ostream& operator<<(std::ostream& os,
				const typename tree<node>::traverser& tt);

// -----------------------------------------------------------------------------
// queries

template <typename... BAs>
bool is_non_terminal(tref n) {
	return tree<node<BAs...>>::get(n).is_nt();
}

template <typename... BAs>
bool is_terminal(tref n) {
	return tree<node<BAs...>>::get(n).is_t();
}

template <typename... BAs>
bool is_non_terminal(const size_t nt, tref n) {
	return tree<node<BAs...>>::get(n).is(nt);
}

template <size_t nt, typename... BAs>
bool is_non_terminal(tref n) { return is_non_terminal<BAs...>(nt, n); }

// factory method for is_non_terminal predicate
template <typename... BAs>
inline std::function<bool(tref)> is_non_terminal(size_t nt) {
	return [nt](tref n) { return is_non_terminal<BAs...>(nt, n); };
}

// -----------------------------------------------------------------------------
// builders:

// definitions of basic bf and wff
const std::string BLDR_BF_0 = "( $X ) =: 0.";
const std::string BLDR_BF_1 = "( $X ) =: 1.";
const std::string BLDR_WFF_F = "( $X ) =:: F.";
const std::string BLDR_WFF_T = "( $X ) =:: T.";

// definitions of wff builder rules
const std::string BLDR_WFF_EQ = "( $X ) =:: $X = 0.";
const std::string BLDR_WFF_NEQ = "( $X ) =:: $X != 0.";
const std::string BLDR_BF_NOT_LESS_EQUAL = "( $X $Y ) =:: $X !<= $Y.";
const std::string BDLR_BF_INTERVAL = "( $X $Y $Z ) =:: $X <= $Y <= $Z.";
const std::string BDLR_BF_NLEQ_UPPER = "( $X $Y ) =:: $X !<= $Y.";
const std::string BDLR_BF_NLEQ_LOWWER = "( $X $Y ) =:: $Y !<= $X.";
const std::string BLDR_WFF_ALL = "( $X $Y ) =:: all $X $Y.";
const std::string BLDR_WFF_EX = "( $X $Y ) =:: ex $X $Y.";
const std::string BLDR_WFF_SOMETIMES = "( $X ) =:: sometimes $X.";
const std::string BLDR_WFF_ALWAYS = "( $X ) =:: always $X.";

// definitions of bf builder rules
const std::string BLDR_BF_SPLITTER = "( $X ) =: S($X).";


// wff builder
template <typename node>
rewriter::builder tree<node>::bldr_wff_eq =
				tree<node>::get_builder(BLDR_WFF_EQ);
template <typename node>
rewriter::builder tree<node>::bldr_bf_splitter =
				tree<node>::get_builder(BLDR_BF_SPLITTER);
template <typename node>
rewriter::builder tree<node>::bldr_bf_not_less_equal =
				tree<node>::get_builder(BLDR_BF_NOT_LESS_EQUAL);
template <typename node>
rewriter::builder tree<node>::bldr_bf_interval =
				tree<node>::get_builder(BDLR_BF_INTERVAL);
template <typename node>
rewriter::builder tree<node>::bldr_bf_nleq_upper =
				tree<node>::get_builder(BDLR_BF_NLEQ_UPPER);
template <typename node>
rewriter::builder tree<node>::bldr_bf_nleq_lower =
				tree<node>::get_builder(BDLR_BF_NLEQ_LOWWER);


} // namespace idni::tau_lang

#include "nso_factory.h"
#include "ba_constants.h"

#include "tau_tree.tmpl.h"
#include "tau_tree_builders.tmpl.h"

#endif // __IDNI__TAU__TAU_TREE_H__