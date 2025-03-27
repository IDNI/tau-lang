#pragma once

#include <variant>

#include "utility/bintree.h"
#include "tau_parser.generated.h"

namespace idni::tau_lang {

// ba constants
struct some_ba {};
struct some_other_ba {};

using ba_constant = std::variant<some_ba, some_other_ba>;

static inline std::vector<ba_constant> C{};         // constants

template <typename BA>
bool is_ba(size_t id) { return std::holds_alternative<BA>(C[id]); }

template <typename BA>
BA get_constant(size_t id) {
	DBG(assert (is_ba<BA>(id));)
	return std::get<BA>(C[id]);
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

	constexpr T extension() const noexcept {
		T result = 0;
		result |= (static_cast<size_t>(nt) & ((1u << node::nt_bits) - 1u)) << node::nt_shift;
		result |= (static_cast<size_t>(term) & 1u) << node::term_shift ;
		result |= (static_cast<size_t>(ba) & ((1u << node::ba_bits) - 1u)) << node::ba_shift;
		result |= (static_cast<size_t>(ext) & 1u) << node::ext_shift;
		result |= static_cast<size_t>(data) & node::data_mask;
		return result;
	}

	auto operator<=>(const node& that) const {
		static constexpr auto cast = [](T x) { return x; };
		if (nt   != that.nt)   return cast(nt)   <=> cast(that.nt);
		if (term != that.term) return cast(term) <=> cast(that.term);
		if (ba   != that.ba)   return cast(ba)   <=> cast(that.ba);
		if (ext  != that.ext)  return cast(ext)  <=> cast(that.ext);
		return data <=> that.data;
	}
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

std::ostream& operator<<(std::ostream& os, const node& n);

template <typename N = node>
struct tree : idni::tree<N> {
	using base_t = idni::tree<N>;
	using node = N;
	using type = node::type;
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

	// creation with node children
	static tref get(const node& v, const node& ch);
	static tref get(const node& v, const node& ch1, const node& ch2);
	static tref get(const node& v, const node* ch, size_t len);
	static tref get(const node& v, const std::vector<node>& ch);

	// creation of node::type and children
	static tref get(const type& nt); // terminal / leaf
	static tref get(const type& nt, tref ch); // with single child
	static tref get(const type& nt, tref ch1, tref ch2); // with two children
	static tref get(const type& nt, const tref* ch, size_t len); // with array of children
	static tref get(const type& nt, const trefs& ch); // with vector of children
	static tref get(const type& nt, const std::string& str); // with string

	// terminals
	static tref get_num(size_t v);
	static tref get_integer(int_t v);
	static tref get_ba_constant_id(size_t v);

	// transform from tau_parser::tree
	static tref get(const tau_parser::tree& t);

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

	const tree& child_tree(size_t n) const;
	const tree& operator[](size_t n) const;
	const tree& first_tree()  const;
	const tree& second_tree() const;
	const tree& third_tree()  const;
	const tree& only_child_tree() const;
	
	std::ostream& print(std::ostream& o, size_t s = 0) const;

	// fast access helpers
	size_t data() const;
	size_t child_data() const;

	bool is(const type& nt) const;
	bool is_string() const;
	bool is_integer() const;
	bool is_num() const;
	bool is_ba_constant() const;

	type get_type() const;
	const std::string& get_string() const;
	int_t get_integer() const;
	size_t get_num() const;
	size_t get_ba_constant_id() const;

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
		traverser(const trefs& n);
		bool has_value() const;
		explicit operator bool() const;
		tref value() const;
		const tree& value_tree() const;
		const tree& operator[](size_t n) const;
		const trefs& values() const;
		std::vector<traverser> traversers() const;
		std::vector<traverser> operator()() const;

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
				tref r = t.value_tree()[0];
				if (!r) return traverser();
				return traverser(r);
			}};
		static inline const extractor<traverser> second{
			[](const traverser& t) {
				if (!t) return traverser();
				tref r = t.value_tree()[1];
				if (!r) return traverser();
				return traverser(r);
			}};
		static inline const extractor<traverser> third{
			[](const traverser& t) {
				if (!t) return traverser();
				tref r = t.value_tree()[2];
				if (!r) return traverser();
				return traverser(r);
			}};
		static inline const extractor<std::string> string{
			[](const traverser& t) {
				if (!t) return std::string();
				return t.value_tree().get_string();
			}};
		static inline const extractor<type> nt {
			[](const traverser& t) {
				if (!t) return static_cast<type>(0);
				return t.value_tree().get_type();
			}};
		static inline const extractor<int_t> integer {
			[](const traverser& t) {
				if (!t) return 0;
				return t.value_tree().get_integer();
			}};
		static inline const extractor<size_t> num {
			[](const traverser& t) -> size_t {
				if (!t) return 0;
				return t.value_tree().get_num();
			}};
		static inline const extractor<size_t> ba_constant_id {
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


};

}

#include "tau.tmpl.h"