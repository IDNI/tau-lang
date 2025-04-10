#pragma once

#include <variant>

#include "defs.h"
#include "utility/tree.h"
#include "tau_parser.generated.h"

namespace idni::tau_lang {

// ba variant
template <typename... BAs>
using ba = std::variant<BAs...>;

static inline std::vector<std::string> ba_types{ "untyped" };
static inline std::unordered_map<std::string, size_t> ba_types_map{
							{ "untyped", 0 } };
template <typename... BAs>
static inline std::vector<ba<BAs...>> C{};         // constants

template <typename BA, typename... BAs>
bool is_ba(size_t id) { return std::holds_alternative<BA>(C<BAs...>[id]); }

template <typename BA, typename... BAs>
BA get_constant(size_t id) { DBG(assert((is_ba<BA, BAs...>(id)));)
	return std::get<BA>(C<BAs...>[id]);
}

// strings dict
static inline std::vector<std::string> S{""};       // strings
static inline std::map<std::string, size_t> SM{};   // string -> id map

struct node {
	using type = tau_parser::nonterminal;
	using T = size_t; // just to simplify changes or templating it later;

	static constexpr size_t bits      = std::numeric_limits<T>::digits;
	static constexpr size_t nt_bits   = tau_parser_data::nt_bits;
	static constexpr size_t ba_bits   = 5;
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

	constexpr node(size_t nt = 0, size_t data = 0, size_t is_term = 0,
			size_t ba_type = 0, size_t ext = 0) noexcept
		: nt(nt), term(is_term), ba(ba_type), ext(ext), data(data),
			hash(hashit()) {}

	int_t as_int() const { return static_cast<int_t>(data); }

	static constexpr node nnull() { return node(); }

	static constexpr node extension(T raw_value) {
		return node(
			(raw_value >> node::nt_shift) & node::nt_mask,
			(raw_value >> node::term_shift) & 1u,
			(raw_value >> node::ba_shift) & ((1u << node::ba_bits) - 1u),
			(raw_value >> node::ext_shift) & 1u,
				raw_value & node::data_mask
		);
	}

#define C(x) static_cast<T>(x)
	constexpr T extension() const noexcept {
		T result = 0;
		result |= (C(nt) & ((1u << node::nt_bits) - 1u)) << node::nt_shift;
		result |= (C(term) & 1u) << node::term_shift ;
		result |= (C(ba) & ((1u << node::ba_bits) - 1u)) << node::ba_shift;
		result |= (C(ext) & 1u) << node::ext_shift;
		result |= C(data) & node::data_mask;
		return result;
	}
	auto operator<=>(const node& that) const {
		if (nt   != that.nt)   return C(nt)   <=> C(that.nt);
		if (term != that.term) return C(term) <=> C(that.term);
		if (ba   != that.ba)   return C(ba)   <=> C(that.ba);
		if (ext  != that.ext)  return C(ext)  <=> C(that.ext);
		return C(data) <=> C(that.data);
	}
#undef C
	constexpr bool operator<(const node& that) const {
		return (*this <=> that) < 0;
	}
	constexpr bool operator<=(const node& that) const {
		return (*this <=> that) <= 0;
	}
	constexpr bool operator>(const node& that) const {
		return (*this <=> that) > 0;
	}
	constexpr bool operator>=(const node& that) const {
		return (*this <=> that) >= 0;
	}
	constexpr auto operator==(const node& that) const {
		return nt == that.nt && term == that.term && ba == that.ba
				&& ext == that.ext && data == that.data;
	}
	constexpr auto operator!=(const node& that) const {
		return !(*this == that);
	}
	constexpr size_t hashit() const {
		std::size_t seed = grcprime;
		hash_combine(seed, static_cast<bool>(nt));
		hash_combine(seed, static_cast<bool>(term));
		hash_combine(seed, ba);
		hash_combine(seed, static_cast<bool>(ext));
		hash_combine(seed, data);
		return seed;
	}
};

inline std::ostream& operator<<(std::ostream& os, const node& n);

template <typename N = node>
struct tree : public idni::lcrs_tree<N>, public tau_parser_nonterminals {
	using base_t = idni::lcrs_tree<N>;
	using node = N;
	using parse_tree = tau_parser::tree;
	using parse_options = tau_parser::parse_options;

	// tree direct API
	// ---------------------------------------------------------------------

	// handles
	tref get() const;
	static const tree& get(const tref id);
	static const tree& get(const htree::sp& h);
	static htree::sp geth(tref id);

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
	static tref get(const node::type& nt, const std::initializer_list<tref>& ch); // with initializer list of children
	static tref get(const node::type& nt, const std::string& str); // with string

	// creation from parser result or parser input (string, stream, file)
	template <typename... BAs>
	static tref get(tau_parser::result& result);
	template <typename... BAs>
	static tref get(const std::string& str, parse_options options = {});
	template <typename... BAs>
	static tref get(std::istream& is, parse_options options = {});
	template <typename... BAs>
	static tref get_from_file(const std::string& filename,
						parse_options options = {});

	// ---------------------------------------------------------------------
	// terminals
	static tref get_num(size_t v);
	static tref get_integer(int_t v);
	static tref get_ba_constant_id(size_t v);

	// transform from tau_parser::tree
	template <typename binder>
	static tref get(const tau_parser::tree& t, binder& b);

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

	const tree& child_tree(size_t n) const;
	const tree& operator[](size_t n) const;
	const tree& first_tree()  const;
	const tree& second_tree() const;
	const tree& third_tree()  const;
	const tree& only_child_tree() const;
	
	std::ostream& print(std::ostream& o) const;
	std::ostream& print_tree(std::ostream& o, size_t s = 0) const;

	// fast access helpers
	size_t data() const;
	size_t child_data() const;

	bool is(size_t nt) const;
	bool is_string() const;
	bool is_integer() const;
	bool is_num() const;
	bool is_ba_constant() const;

	node::type get_type() const;
	const std::string& get_type_name() const;
	const std::string& get_string() const;
	int_t get_integer() const;
	size_t get_num() const;
	size_t get_ba_constant_id() const;

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
		bool empty() const;
		size_t size() const;

		template <node::type NT>
		static inline const extractor<bool> is{
			[](const traverser& t) {
				if (!t) return false;
				return t.value_tree().get_type() == NT;
			}};
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
		static inline const extractor<size_t> ba_constant_id{
			[](const traverser& t) -> size_t {
				if (!t) return 0;
				return t.value_tree().get_ba_constant_id();
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
	traverser operator|(size_t nt) const;
	traverser operator||(size_t nt) const;

	using tt = traverser;

	// ---------------------------------------------------------------------
	// library and builders

	static rewriter::library get_library(const std::string& str);
	static rewriter::builder get_builder(tref ref);
	static rewriter::builder get_builder(const std::string& source);

	static tref apply_builder(const rewriter::builder& b, trefs n);

	static htree::sp _0;
	static htree::sp _0_trimmed;
	static htree::sp _1;
	static htree::sp _1_trimmed;
	static htree::sp _F;
	static htree::sp _F_trimmed;
	static htree::sp _T;
	static htree::sp _T_trimmed;

	struct build {

		static tref variable(const std::string& name);
		static tref in_var_name(size_t index);
		static tref type(const std::string& type);
		static tref bf_t_type(const std::string& type);
		static tref bf_t_type(tref type);
		static tref bf_f_type(const std::string& type);
		static tref bf_f_type(tref type);
		static tref in_variable_at_n(tref in_var_name, size_t num);
		static tref in_variable_at_n(size_t index, size_t num);
		static tref in_variable_at_n(const std::string& name, int_t pos);
		static tref in_variable_at_t(tref in_var_name);
		static tref in_variable_at_t(size_t index);
		static tref in_variable_at_t_minus(const std::string& var_name,
									int_t shift);
		static tref in_variable_at_t_minus(tref in_var_name, size_t num);
		static tref in_variable_at_t_minus(size_t index, size_t num);
		static tref out_var_name(size_t index);
		static tref out_var_name(const std::string& name);
		static tref out_variable_at_t(tref out_var_name);
		static tref out_variable_at_t(size_t index);
		static tref out_variable_at_n(tref out_var_name, size_t num);
		static tref out_variable_at_n(size_t index, size_t num);
		static tref out_variable_at_n(const std::string& name, int_t pos);
		static tref out_variable_at_t_minus(const std::string& out_var_name,
									int_t shift);
		static tref out_variable_at_t_minus(tref out_var_name, size_t num);
		static tref out_variable_at_t_minus(size_t index, size_t num);
		// static tref bf_variable(const std::string& name);
		static tref bf_constant(tref cte);
		static tref bf_constant(tref cte, tref type);
		static tref bf_constant(tref cte, const std::string& type);
		
		// build_bf_and_constant and build_bf_or_constant

		static tref bf_uninter_const(const std::string& name1, const std::string& name2);
		static tref wff_eq(tref l, tref r);
		static tref wff_eq(tref l);
		static tref wff_neq(tref l);
		static tref wff_and(tref l, tref r);
		static tref wff_and(const auto& wffs);
		static tref wff_or(tref l, tref r);
		static tref wff_or(const auto& wffs);
		static tref wff_neg(tref n);
		static tref wff_xor_from_def(tref l, tref r);
		static tref wff_xor(tref l, tref r);
		static tref wff_imply(tref l, tref r);
		static tref wff_conditional(tref x, tref y, tref z);
		static tref wff_equiv(tref l, tref r);
		static tref wff_all(tref l, tref r);
		static tref wff_ex(tref l, tref r);
		static tref wff_sometimes(tref n);
		static tref wff_always(tref n);
		static tref bf_and(tref l, tref r);
		static tref bf_and(const auto& bfs);
		static tref bf_or(tref l, tref r);
		static tref bf_or(const auto& bfs);
		static tref bf_neg(tref n);
		static tref bf_xor_from_def(tref l, tref r);
		static tref bf_xor(tref l, tref r);
		static tref bf_less(tref l, tref r);
		static tref bf_nless(tref l, tref r);
		static tref bf_less_equal(tref l, tref r);
		static tref bf_nleq(tref l, tref r);
		static tref bf_interval(tref x, tref y, tref z);
		static tref bf_nleq_lower(tref l, tref r);
		static tref bf_nleq_upper(tref l, tref r);
		static tref bf_greater(tref l, tref r);
		static tref bf_ngreater(tref l, tref r);
		static tref bf_greater_equal(tref l, tref r);
		static tref bf_ngeq(tref l, tref r);
		static tref wff_ctn_greater_equal(tref ctnvar, tref num);
		static tref wff_ctn_greater(tref ctnvar, tref num);
		static tref wff_ctn_less_equal(tref ctnvar, tref num);
		static tref wff_ctn_less(tref ctnvar, tref num);
		static tref wff_ctn_eq(tref ctnvar, tref num);
		static tref wff_ctn_neq(tref ctnvar, tref num);
	};	
};

template <typename node>
bool is_nt(tref n) {
	return tree<node>::get(n).is_nt();
}

template <typename node>
bool is_t(tref n) {
	return tree<node>::get(n).is_t();
}

template <typename node>
bool is(const size_t nt, tref n) {
	return tree<node>::get(n).is(nt);
}

template <typename node, size_t nt>
bool is(tref n) { return is<node>(nt, n); }

// factory method for is predicate
template <typename node>
inline std::function<bool(tref)> is(size_t nt) {
	return [nt](tref n) { return is<node>(nt, n); };
}


// -----------------------------------------------------------------------------
// builders:

template <typename node>
htree::sp tree<node>::_0_trimmed = tree<node>::get(node::type::bf_t);
template <typename node>
htree::sp tree<node>::_0 = tree<node>::get(node::type::bf, tree<node>::_0_trimmed);

template <typename node>
htree::sp tree<node>::_1_trimmed = tree<node>::get(node::type::bf_t);
template <typename node>	
htree::sp tree<node>::_1 = tree<node>::get(node::type::bf, tree<node>::_1_trimmed);

template <typename node>
htree::sp tree<node>::_F_trimmed = tree<node>::get(node::type::wff_f);
template <typename node>
htree::sp tree<node>::_F = tree<node>::get(node::type::wff, tree<node>::_F_trimmed);

template <typename node>
htree::sp tree<node>::_T_trimmed = tree<node>::get(node::type::wff_t);
template <typename node>
htree::sp tree<node>::_T = tree<node>::get(node::type::wff, tree<node>::_T_trimmed);


const std::string BUILDER_BF_NLEQ_UPPER = "( $X $Y ) =:: $X !<= $Y.";
const std::string BUILDER_BF_NLEQ_LOWER = "( $X $Y ) =:: $Y !<= $X.";
// // wff builder
// template <typename node>
// inline static auto builder_wff_eq = tree<node>::get_builder(BLDR_WFF_EQ);
// template <typename node>
// inline static auto builder_bf_splitter = tree<node>::get_builder(BLDR_BF_SPLITTER);
// template <typename node>
// inline static auto builder_bf_not_less_equal =
// 				tree<node>::get_builder(BLDR_BF_NOT_LESS_EQUAL);
// template <typename node>
// inline static auto builder_bf_interval = tree<node>::get_builder(BDLR_BF_INTERVAL);
template <typename node>
static auto builder_bf_nleq_upper = tree<node>::get_builder(BUILDER_BF_NLEQ_UPPER);
template <typename node>
static auto builder_bf_nleq_lower = tree<node>::get_builder(BUILDER_BF_NLEQ_LOWER);

// // basic bf and wff constants
// template <typename node>
// inline static htree::sp _0            = builder_bf_0<node>.second;
// template <typename node>
// inline static htree::sp _0_trimmed    = trim<node>(_0<node>);
// template <typename node>
// inline static htree::sp _1            = builder_bf_1<node>.second;
// template <typename node>
// inline static htree::sp _1_trimmed    = trim<node>(_1<node>);
// template <typename node>
// inline static htree::sp _F            = builder_wff_F<node>.second;
// template <typename node>
// inline static htree::sp _F_trimmed    = trim<node>(_F<node>);
// template <typename node>
// inline static htree::sp _T            = builder_wff_T<node>.second;
// template <typename node>
// inline static htree::sp _T_trimmed    = trim<node>(_T<node>);

}

#include "tau.tmpl.h"
#include "tau_builders.h"