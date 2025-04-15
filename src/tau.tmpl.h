// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "nso_rr.h"

namespace idni::tau_lang {

inline bool pretty_printer_highlighting = false;
inline bool pretty_printer_indenting    = false;

// node
// -----------------------------------------------------------------------------

template <typename... BAs>
constexpr node<BAs...> node<BAs...>::ba_constant(size_t v, size_t ba_tid) {
	return node(type::bf_constant, v, true /*is_term*/, ba_tid);
}

template <typename... BAs>
constexpr node<BAs...>::node(size_t nt, size_t data, size_t is_term,
		size_t ba_type, size_t ext) noexcept
	: nt(nt), term(is_term), ba(ba_type), ext(ext), data(data),
		hash(hashit()) {}

template <typename... BAs>
int_t node<BAs...>::as_int() const { return static_cast<int_t>(data); }

template <typename... BAs>
constexpr node<BAs...> node<BAs...>::nnull() { return node(); }

template <typename... BAs>
constexpr node<BAs...> node<BAs...>::extension(T raw_value) {
	return node(
		(raw_value >> node::nt_shift) & node::nt_mask,
		(raw_value >> node::term_shift) & 1u,
		(raw_value >> node::ba_shift) & ((1u << node::ba_bits) - 1u),
		(raw_value >> node::ext_shift) & 1u,
			raw_value & node::data_mask
	);
}

#define C(x) static_cast<T>(x)
template <typename... BAs>
constexpr node<BAs...>::T node<BAs...>::extension() const noexcept {
	T result = 0;
	result |= (C(nt) & ((1u << node::nt_bits) - 1u)) << node::nt_shift;
	result |= (C(term) & 1u) << node::term_shift ;
	result |= (C(ba) & ((1u << node::ba_bits) - 1u)) << node::ba_shift;
	result |= (C(ext) & 1u) << node::ext_shift;
	result |= C(data) & node::data_mask;
	return result;
}
template <typename... BAs>
auto node<BAs...>::operator<=>(const node& that) const {
	if (nt   != that.nt)   return C(nt)   <=> C(that.nt);
	if (term != that.term) return C(term) <=> C(that.term);
	if (ba   != that.ba)   return C(ba)   <=> C(that.ba);
	if (ext  != that.ext)  return C(ext)  <=> C(that.ext);
	return C(data) <=> C(that.data);
}
#undef C
template <typename... BAs>
constexpr bool node<BAs...>::operator<(const node& that) const {
	return (*this <=> that) < 0;
}
template <typename... BAs>
constexpr bool node<BAs...>::operator<=(const node& that) const {
	return (*this <=> that) <= 0;
}
template <typename... BAs>
constexpr bool node<BAs...>::operator>(const node& that) const {
	return (*this <=> that) > 0;
}
template <typename... BAs>
constexpr bool node<BAs...>::operator>=(const node& that) const {
	return (*this <=> that) >= 0;
}
template <typename... BAs>
constexpr auto node<BAs...>::operator==(const node& that) const {
	return nt == that.nt && term == that.term && ba == that.ba
			&& ext == that.ext && data == that.data;
}
template <typename... BAs>
constexpr auto node<BAs...>::operator!=(const node& that) const {
	return !(*this == that);
}
template <typename... BAs>
constexpr size_t node<BAs...>::hashit() const {
	std::size_t seed = grcprime;
	hash_combine(seed, static_cast<bool>(nt));
	hash_combine(seed, static_cast<bool>(term));
	hash_combine(seed, ba);
	hash_combine(seed, static_cast<bool>(ext));
	hash_combine(seed, data);
	return seed;
}


template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const node<BAs...>& n) {
	using tau = tree<node<BAs...>>;
	if (n.nt == tau_parser::integer) os << n.as_int() << " ";
	else if (n.nt == tau_parser::bf_constant) os << " {C" << n.data << "} ";
	else if (tau::is_digital_nt(n.nt)) os << n.data << " ";
	else if (tau::is_string_nt(n.nt)) os << "\"" << string_from_id(n.data) << "\" ";
	// else if (n.ext) os << "{EXT}";
	os << tau_parser::instance().name(n.nt);
#ifdef DEBUG
	if (bool print_nt_ids = true; print_nt_ids) os << "(" << n.nt << ")";
	if (n.data) os << " #" << n.data;
#endif
	return os;
}

//------------------------------------------------------------------------------
// transform from tau_parser::tree

// Converts parser tree nodes to tau tree nodes
// - terminals are collapsed into their parents
//     ie. shift > num > "10" becomes shift > num("10")
//     or  variable > type > "sbf" becomes variable > type("sbf")
// - does q_vars transformation from ex x, y to ex x ex y
// - transforms offset variables to captures
// important terminals are stored in their pools outside of the tree
// and node's value is actually id in the pool (strings, constants)
// for constants,
//     binder requires type and a tref. binder stores it and returns its id

// post_order search of a parse tree.
//
// when a node is reached, all its children are already transformed and stored
// in the map `m` - parse tree node ref -> tau tree node ref.
// each node needs to be transformed (usually just value is copied) and
// children has to be remapped through the map.
//
// See default in the transformer's switch for example of usual transformation.
// 

template <typename node>
template <typename binder>
tref tree<node>::get(binder& bind, const tau_parser::tree& ptr) {
	using type = typename node::type;

	// map of parse tree nodes' refs to tau tree nodes' refs
	std::unordered_map<tref, tref> m;
	// exist in map (is already transformed)
	auto m_ex = [&m](tref t) { return m.find(t) != m.end(); };
	// get tau tree node ref from parse tree node ref
	auto m_ref = [&m](tref t) { return m.at(t); };
	// get tau tree node instance from parse tree node ref
	auto m_get = [&m](tref t) { return get(m.at(t)); };

	std::string src; // source of the current constant
	bool error = false;

	auto transformer = [&m, &m_ex, &m_ref, &m_get, &bind, &src,&error](
		tref t, tref parent)
	{
		// parse_tree::get(t).dump(std::cout << "transforming: ", true) << std::endl;

		if (m_ex(t)) return true; // already transformed
		const auto& ptr = parse_tree::get(t); // get parse tree node
		if (ptr.is_t()) return true; // terminals are processed by their parents

		// get node type and parent node type
		type nt = static_cast<type>(ptr.get_nt());
		type parent_nt = static_cast<type>(parent == nullptr ? 0
					: parse_tree::get(parent).get_nt());

		// q_vars transformation (ex x,y to ex x ex y)
		auto process_quantifier_vars = [&ptr, &m_get, &nt]() -> tref {
			const auto& q_vars = m_get(ptr[0]);
			const auto& wff_expr = m_get(ptr[1]);
			trefs vars = q_vars.get_children();
			if (vars.empty()) // TODO (LOW) is this even reachable?
				return get(node(nt), {
						q_vars.get(), wff_expr.get() });
			tref x = wff_expr.only_child();
			for (size_t vi = 0; vi != vars.size(); ++vi) {
				// create a new quantifier node with var and new
				// children. Note the reversed order!
				x = get(node(nt), {
					vars[vars.size() - 1 - vi],
					get(wff, x) });
			}
			return x;
		};

		// takes data value from a preprocessed digit subnode
		// and negates if '-' sign is parsed (if children size == 2)
		auto process_integer = [&ptr, &m_get]() -> int_t {
			trefs ch = ptr.get_children(); // 1 or 2
			bool neg = ch.size() == 2;
			int_t i = static_cast<int_t>( // get data from child 0 or 1)
				m_get(ch[static_cast<size_t>(neg)]).data());
			return neg ? -i : i;
		};

		// simple renaming transformations
		// TODO (LOW) this should be supported directly by the parser
		auto retype_nonterminal = [&nt, &parent_nt]() -> node::type {
			switch (nt) { // of intermediate nodes
				// bf_and_nosep and bf_neg_oprnd
				case bf_and_nosep_1st_oprnd:
				case bf_and_nosep_2nd_oprnd:
				case bf_neg_oprnd:
					nt = bf; break;
				case bf_and_nosep:
					nt = bf_and; break;
				// offset variable -> capture
				case variable:
					if (parent_nt == offsets || parent_nt == ref_arg)
						nt = capture;
					break;
				default: break;
			}
			return static_cast<node::type>(nt);
		};

		auto get_type = [](tref n) -> std::string {
			auto t = tt(n) | tau_parser::type;
			if (t) {
				t.value_tree().dump(std::cout << "type1: ") << std::endl;
				std::cout << t.value() << "\n";
				std::cout << t.value_tree().data() << "\n";
				std::cout << "type2: " << t.value_tree().get_string() << std::endl;
				std::cout << "type3: " << (t | tt::string) << std::endl;
				return t | tt::string;
			}
			return "";
		};

		tref x; // result of node transformation
		switch (nt) {
			// tau tree terminals
			case digits: // preprocess digits
				// TODO: ??? check if number fits into data bitsize, if not, create an ext node
				// maybe only if --ext_nodes option is used?
				x = get(nt, std::stoul(ptr.get_terminals()));
				break;

			case integer: x = get_integer(process_integer()); break;

			// digital terminals (same as is_digital_nt())
			case num:
			case memory_id:
				x = get(node(nt), m_get(ptr.only_child()).data());
				break;

			case wff_all:
			case wff_ex: x = process_quantifier_vars(); break;

			// preprocess source node
			case source:
				src = ptr.get_terminals();
				std::cout << "source: " << src << std::endl;
				x = get(node(source), (size_t) 0);
				break;

			default:
				if (is_string_nt(nt)) {
					x = get(static_cast<node::type>(nt),
							ptr.get_terminals());
					break;
				}

				// arbitrary nonterminal
				trefs ch;
				for (tref c : ptr.children()) {
					DBG(assert(c != nullptr && m_ex(c));)
					ch.push_back(m_ref(c));
				}
				// retype if needed
				x = get(retype_nonterminal(), ch);

				// call binder on a transformed bf_constant node
				if (nt == bf_constant) {
					if (!get(x)[0][0].is(binding)) break; // capture?
					auto nn = bind(src, get_type(x));
					if (nn == nullptr || bind.error || nn == x)
						return error = true, false;
					x = nn;
				}
				break;
		}

		m.emplace(t, x); // store new node into map

		// std::cout << "emplaced:" << parse_tree::get(t).value
		// 	<< " as " << tree::get(x).get_type_name() << std::endl;
		return true;
	};
	// parse_tree::get(pt.get()).print(std::cout << "parse tree: ") << std::endl;
	post_order<tau_parser::pnode>(ptr.get()).search(transformer);
	if (error) return nullptr;
	if (m.find(ptr.get()) == m.end()) return nullptr;
	// std::cout << "tau tree: " << tree::get(m.at(pt.get())) << std::endl;
	// tree::get(m_get(pt.get())).print_tree(std::cout << "tau tree: ") << std::endl;
	return m_ref(ptr.get());
}

template <typename N>
tref tree<N>::get(const tau_parser::tree& pt) {
	return get<ba_constants_t>(ba_constants_t::instance(), pt);
}

//------------------------------------------------------------------------------
// handles

template <typename node>
tref tree<node>::get() const { return base_t::get(); }
template <typename node>
const tree<node>& tree<node>::get(const tref id) {
	return (const tree&) base_t::get(id);
}
template <typename node>
const tree<node>& tree<node>::get(const htree::sp& h) {
	return (const tree&) base_t::get(h);
}
template <typename node>
htree::sp tree<node>::geth(tref h) {return base_t::geth(h);}

//------------------------------------------------------------------------------
// creation with tref childs

template <typename node>
tref tree<node>::get(const node& v) { return base_t::get(v); }

template <typename node>
tref tree<node>::get(const node& v, tref child) {
	return base_t::get(v, child);
}

template <typename node>
tref tree<node>::get(const node& v, tref ch1, tref ch2) {
	return base_t::get(v, ch1, ch2);
}

template <typename node>
tref tree<node>::get(const node& v, const tref* ch, size_t len) {
	return base_t::get(v, ch, len);
}

template <typename node>
tref tree<node>::get(const node& v, const trefs& children) {
	return base_t::get(v, children);
}

template <typename node>
tref tree<node>::get(const node& v, const std::initializer_list<tref>& ch) {
	return base_t::get(v, ch);
}


//------------------------------------------------------------------------------
// creation with node childs

template <typename node>
tref tree<node>::get(const node& v, const node& child) {
	return base_t::get(v, child);
}

template <typename node>
tref tree<node>::get(const node& v, const node& ch1, const node& ch2)
{
	return get(v, ch1, ch2);
}

template <typename node>
tref tree<node>::get(const node& v, const node* ch, size_t len)
{
	return base_t::get(v, ch, len);
}

template <typename node>
tref tree<node>::get(const node& v, const std::vector<node>& ch)
{
	return base_t::get(v, ch);
}

template <typename node>
tref tree<node>::get(const node& v, const std::initializer_list<node>& ch)
{
	return base_t::get(v, ch);
}

//------------------------------------------------------------------------------
// creation of node::type and children

template <typename node>
tref tree<node>::get(const node::type& nt) { return get(node(nt)); }

template <typename node>
tref tree<node>::get(const node::type& nt, tref ch) {
	return get(node(nt), ch);
}

template <typename node>
tref tree<node>::get(const node::type& nt, tref ch1, tref ch2) {
	return get(node(nt), ch1, ch2);
}

template <typename node>
tref tree<node>::get(const node::type& nt, const tref* ch, size_t len) {
	return get(node(nt), ch, len);
}

template <typename node>
tref tree<node>::get(const node::type& nt, const trefs& ch) {
	return get(node(nt), ch);
}

template <typename node>
tref tree<node>::get(const node::type& nt, const std::initializer_list<tref>& ch) {
	return get(node(nt), ch);
}

template <typename node>
tref tree<node>::get(const node::type& nt, const std::string& s) {
	return get(node(nt, string_id(s)));
}

//------------------------------------------------------------------------------
// terminals

template <typename node>
tref tree<node>::get_num(size_t n) {
	return get(node(tree<node>::num, n));
}

template <typename node>
tref tree<node>::get_integer(int_t n) {
	return get(node(integer, static_cast<size_t>(n)));
}

template <typename node>
tref tree<node>::get_ba_constant(size_t c) {
	return get_ba_constant(c, 0);
}

template <typename node>
tref tree<node>::get_ba_constant(size_t c, size_t tid) {
	return get(node::ba_constant(c, tid));
}

//------------------------------------------------------------------------------

template <typename N>
template <typename binder>
tref tree<N>::get(binder& bind, tau_parser::result& result) {
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
		return nullptr; // Syntax error
	}
	auto pt = parse_tree::get(result.get_shaped_tree2());
	return tree<N>::template get<binder>(bind, pt);
}

template <typename N>
tref tree<N>::get(tau_parser::result& result) {
	return tree<N>::get<ba_constants_t>(ba_constants_t::instance(), result);
}

template <typename N>
template <typename binder>
tref tree<N>::get(binder& bind, const std::string& source, parse_options options) {
	auto result = tau_parser::instance()
		.parse(source.c_str(), source.size(), options);
	return tree<N>::template get<binder>(bind, result);
}

template <typename N>
tref tree<N>::get(const std::string& source, parse_options options) {
	return tree<N>::get<ba_constants_t>(
			ba_constants_t::instance(), source, options);
}

template <typename N>
template <typename binder>
tref tree<N>::get(binder& bind, std::istream& is, parse_options options) {
	auto result = tau_parser::instance().parse(is, options);
	return tree<N>::template get<binder>(bind, result);
}

template <typename N>
tref tree<N>::get(std::istream& is, parse_options options) {
	return tree<N>::get<ba_constants_t>(
		ba_constants_t::instance(), is, options);
}

template <typename N>
template <typename binder>
tref tree<N>::get_from_file(binder& bind, const std::string& filename, parse_options options){
	auto result = tau_parser::instance().parse(filename, options);
	return tree<N>::template get<binder>(bind, result);
}

template <typename N>
tref tree<N>::get_from_file(const std::string& filename, parse_options options){
	return tree<N>::get<ba_constants_t>(
		ba_constants_t::instance(), filename, options);
}

template <typename N>
rewriter::rules tree<N>::get_rec_relations(tref rrs) {
	rewriter::rules x;
	auto t = tt(rrs);
	if (t.is(rec_relation))
		return x.emplace_back(geth(t.value_tree().first()),
					geth(t.value_tree().second())), x;
	if (t.is(spec)) t = t | rec_relations;
	t = t || rec_relation;
	for (auto& r : t())
		x.emplace_back(r | tt::first | tt::handle,
				r | tt::second | tt::handle);
	return x;
}

template <typename N>
void tree<N>::get_leaves(tref n, node::type branch, trefs& leaves) {
	auto add_leave = [&branch, &leaves](tref n) {
		const auto& t = tree<N>::get(n);
		if (t.is(branch)) return true;
		if (t.child_is(branch)) return true;
		return leaves.push_back(n), false;
	};
	pre_order<N>(n).visit(add_leave);
}

template <typename N>
trefs tree<N>::get_leaves(tref n, node::type branch) {
	trefs leaves;
	get_leaves(n, branch, leaves);
	return leaves;
}

template <typename N>
trefs tree<N>::get_dnf_wff_clauses(tref n) {
	return get_leaves(n, tau_parser::wff_or);
}

template <typename N>
trefs tree<N>::get_dnf_bf_clauses(tref n) {
	return get_leaves(n, tau_parser::bf_or);
}

template <typename N>
trefs tree<N>::get_cnf_wff_clauses(tref n) {
	return get_leaves(n, tau_parser::wff_and);
}

template <typename N>
trefs tree<N>::get_cnf_bf_clauses(tref n) {
	return get_leaves(n, tau_parser::bf_and);
}

template <typename N>
rewriter::rules tree<N>::get_rules(tref r) {
	auto rs = tt(r) | rules || rule || tt::first;
	rewriter::rules x;
	for (auto r : rs.traversers()) {
		// tree::get(r.value()).print(std::cout << "rule: ");
		x.emplace_back(tree::geth(r | tt::first | tt::ref),
				tree::geth(r | tt::second | tt::ref));
	}
	return x;
}

template <typename N>
template <typename binder>
rewriter::library tree<N>::get_library(binder& bind, const std::string& str) {
	return get_rules(tree::get<binder>(bind, str, { .start=library }));
}

template <typename N>
rewriter::library tree<N>::get_library(const std::string& str) {
	return tree<N>::get_library<ba_constants_t>(
		ba_constants_t::instance(), str);
}

template <typename N>
template <typename binder>
rewriter::builder tree<N>::get_builder(binder& bind, const std::string& source) {
	return tree<N>::get_builder(get<binder>(bind, source, { .start = tau_parser::builder }));
}

template <typename N>
rewriter::builder tree<N>::get_builder(const std::string& source) {
	return tree<N>::get_builder<ba_constants_t>(
		ba_constants_t::instance(), source);
}

template <typename N>
rewriter::builder tree<N>::get_builder(tref ref) {
	DBG(assert(ref != nullptr);)
	tt b(ref);
	auto body = b | builder_body | tt::only_child;
	DBG(assert((body | tt::nt) == bf_builder_body
		|| (body | tt::nt) == wff_builder_body);)
	return { geth(b | builder_head | tt::ref),
		geth(body | tt::first | tt::ref) };
}

template <typename node>
std::optional<rr> tree<node>::get_nso_rr(tref r) {
	using tt = tree<node>::traverser;
	const auto& t = get(r);
	if (t.is(bf) || t.is(ref)) return { { {}, geth(r) } };
	if (t.is(rec_relation))
		return { { tree<node>::get_rec_relations(r), geth(r) } };
	tref main_fm = (t.is(tau_constant_source) || t.is(spec)
					? tt(r) | main
					: tt(r) | spec | main) | wff | tt::ref;
	rewriter::rules rules = get_rec_relations(r);
	return { { rules, geth(main_fm) } };
}


//------------------------------------------------------------------------------

template <typename node>
size_t tree<node>::children_size() const {
	return base_t::children_size();
}

template <typename node>
bool tree<node>::get_children(tref *child, size_t& len) const {
	return base_t::get_children(child, len);
}

template <typename node>
tref_range<node> tree<node>::children() const {
	return base_t::children();
}

template <typename node>
tree_range<tree<node>> tree<node>::children_trees() const{
	return tree_range<tree<node>>(this->l);
}

template <typename node>
tref tree<node>::only_child() const {
	return base_t::only_child();
}

template <typename node>
trefs tree<node>::get_children() const {
	return base_t::get_children();
}

template <typename node>
tref tree<node>::child(size_t n) const { return base_t::child(n);}

template <typename node>
tref tree<node>::first() const { return child(0); }

template <typename node>
tref tree<node>::second() const { return child(1); }

template <typename node>
tref tree<node>::third() const { return child(2); }

template <typename node>
const tree<node>& tree<node>::child_tree(size_t n) const {
	tref c = child(n); DBG(assert(c != nullptr);)
	return get(c);
}

template <typename node>
const tree<node>& tree<node>::operator[](size_t n) const {
	return child_tree(n);
}

template <typename node>
const tree<node>& tree<node>::first_tree() const {
	return child_tree(0);
}

template <typename node>
const tree<node>& tree<node>::second_tree() const {
	return child_tree(1);
}

template <typename node>
const tree<node>& tree<node>::third_tree() const {
	return child_tree(2);
}

template <typename node>
const tree<node>& tree<node>::only_child_tree() const {
	tref c = only_child(); DBG(assert(c != nullptr);)
	return get(c);
}



template <typename node>
std::ostream& tree<node>::print_tree(std::ostream& os, size_t s) const {
	for (size_t i = 0; i < s; i++) os << "\t";
	os << this->value;
	// DBG(os << " *" << get();)
	os << "\n";
	for (const auto& ch : children()) get(ch).print_tree(os, s + 1);
	return os;
}

//------------------------------------------------------------------------------
// nt category helpers

template <typename node>
bool tree<node>::is_string_nt(size_t nt) {
	static const std::set<size_t> string_nts{
		sym, type, source, named_binding, capture, variable,
		in_var_name, out_var_name, uninter_const_name, file_name,
		ctnvar, option_name, option_value
	};
	return string_nts.contains(nt);
}

template <typename node>
bool tree<node>::is_digital_nt(size_t nt) {
	static const std::set<size_t> digital_nts{
		num, memory_id
	};
	return digital_nts.contains(nt);
}

//------------------------------------------------------------------------------
// fast access helpers

template <typename node>
size_t tree<node>::data() const { return this->value.data; }

template <typename node>
size_t tree<node>::child_data() const { return first_tree().value.data; }

template <typename node>
bool tree<node>::is(size_t nt) const {
	return this->value.nt == nt;
}

template <typename node>
bool tree<node>::is_string() const { return is_string_nt(this->value.nt); }

template <typename node>
bool tree<node>::is_integer() const { return is(integer); }

template <typename node>
bool tree<node>::is_num() const { return is(num); }

template <typename node>
bool tree<node>::is_ba_constant() const { return is(bf_constant); }

template <typename node>
bool tree<node>::child_is(size_t nt) const {
	return first_tree().is(nt);
}

template <typename node>
node::type tree<node>::get_type() const {
	return static_cast<node::type>(this->value.nt);
}

template <typename node>
const std::string& tree<node>::get_type_name() const {
	return tau_parser::instance().name(this->value.nt);
}

template <typename node>
const std::string& tree<node>::get_string() const {
	DBG(assert(is_string());)
	return string_from_id(this->value.data);
}

template <typename node>
int_t tree<node>::get_integer() const {
	DBG(assert(is_integer());)
	return static_cast<int_t>(this->value.data);
}

template <typename node>
size_t tree<node>::get_num() const {
	DBG(assert(is_num());)
	return this->value.data;
}


template <typename node>
size_t tree<node>::get_ba_constant_id() const {
	DBG(assert(is_ba_constant());)
	return this->value.data;
}

template <typename node>
tree<node>::bas_variant tree<node>::get_ba_constant() const {
	DBG(assert(is_ba_constant());)
	return ba_constants_t::get(data());
}

//------------------------------------------------------------------------------

template <typename node>
tree<node>::traverser::traverser() : has_value_(false) {}
template <typename node>
tree<node>::traverser::traverser(tref r) : has_value_(r != nullptr),
			values_(has_value_ ? trefs{ r } : trefs{}) {}
template <typename node>
tree<node>::traverser::traverser(const htree::sp& h) : has_value_(h != nullptr),
			values_(has_value_ ? trefs{ h->get() } : trefs{}) {}
template <typename node>
tree<node>::traverser::traverser(const trefs& refs) { set_values(refs); }

template <typename node>
bool tree<node>::traverser::has_value() const { return has_value_; }
template <typename node>
tree<node>::traverser::operator bool() const { return has_value(); }

template <typename node>
tref tree<node>::traverser::value() const { return values_.front(); }

template <typename node>
const tree<node>& tree<node>::traverser::value_tree()
	const { return get(values_.front()); }

template <typename node>
const tree<node>& tree<node>::traverser::operator[](
	size_t n) const { return value_tree()[n]; }

template <typename node>
const trefs& tree<node>::traverser::values() const { return values_; }

template <typename node>
std::vector<typename tree<node>::traverser>
	tree<node>::traverser::traversers() const
{
	std::vector<traverser> tv;
	for (const auto& v : values_) tv.emplace_back(v);
	return tv;
}

template <typename node>
std::vector<typename tree<node>::traverser>
	tree<node>::traverser::operator()() const
{
	return traversers();
}

template <typename node>
bool tree<node>::traverser::empty() const { return values_.empty(); }

template <typename node>
size_t tree<node>::traverser::size() const { return values_.size(); }

template <typename node>
typename tree<node>::traverser
	tree<node>::traverser::operator|(size_t nt) const
{
	if (!has_value()) return traverser();
	for (tref c : get(value()).children()) {
		if (get(c).is(nt)) return { c }; 
	}
	return {};
}

template <typename node>
typename tree<node>::traverser
	tree<node>::traverser::operator||(size_t nt) const
{
	trefs r;
	for (tref v : values())	for (tref c : get(v).children())
			if (get(c).is(nt)) r.push_back(c);
	return traverser(r);
}

template <typename node>
template <typename result_type>
result_type tree<node>::traverser::operator|(
	const extractor<result_type>& e) const
{
	return e(*this);
}

template <typename node>
template <typename result_type>
result_type tree<node>::traverser::operator||(
	const extractor<result_type>& e) const
{
	return e(*this);
}

template <typename node>
void tree<node>::traverser::set_values(const trefs& refs) {
	for (tref t : refs) if (t != nullptr) values_.push_back(t);
	has_value_ = values_.size();
}

//------------------------------------------------------------------------------
// print
template <typename node>
std::ostream& tree<node>::print(std::ostream& os) const {

// TODO wff_all, wff_ex (print all x all y as all x, y)
// TODO bf_and spacing
// TODO review indenting and syntax highlighting

// #define DEBUG_TRAVERSAL 1

	std::vector<size_t> hl_path;
	size_t depth = 0;
	std::unordered_map<tref, size_t> chpos;
	std::unordered_set<tref> wraps, indented, highlighted;
	auto is_to_wrap = [](size_t nt, size_t pt) {
		static const std::set<size_t> no_wrap_for = {
			bf_splitter, bf_ref, bf_neg, bf_constant, bf_t, bf_f,
			wff_ref, wff_neg, wff_t, wff_f, constraint, capture,
			variable, uninterpreted_constant, ref_args, start
		};
		// priority map (lower number = higher priority)
		static const std::map<size_t, size_t> prio = {
			{ start,                0 },
			{ help_cmd,            50 },
			{ file_cmd,            50 },
			{ normalize_cmd,       50 },
			{ run_cmd,             50 },
			{ solve_cmd,           50 },
			{ dnf_cmd,             50 },
			{ cnf_cmd,             50 },
			{ anf_cmd,             50 },
			{ nnf_cmd,             50 },
			{ pnf_cmd,             50 },
			{ mnf_cmd,             50 },
			{ onf_cmd,             50 },
			{ inst_cmd,            50 },
			{ subst_cmd,           50 },
			{ def_rr_cmd,          50 },
			{ def_list_cmd,        50 },
			{ history_list_cmd,    50 },
			{ history_print_cmd,   50 },
			{ history_store_cmd,   50 },
			{ sat_cmd,             50 },
			{ main,                60 },
			{ bf_rule,             60 },
			{ wff_rule,            60 },
			{ ref,                 80 },
			{ wff_sometimes,      380 },
			{ wff_always,         390 },
			{ wff_conditional,    400 },
			{ wff_all,            430 },
			{ wff_ex,             440 },
			{ wff_imply,          450 },
			{ wff_equiv,          460 },
			{ wff_or,             470 },
			{ wff_and,            480 },
			{ wff_xor,            490 },
			{ wff_neg,            500 },
			{ bf_interval,        501 },
			{ bf_eq,              502 },
			{ bf_neq,             503 },
			{ bf_less_equal,      504 },
			{ bf_nleq,            505 },
			{ bf_greater,         506 },
			{ bf_ngreater,        507 },
			{ bf_greater_equal,   508 },
			{ bf_ngeq,            509 },
			{ bf_less,            510 },
			{ bf_nless,           511 },
			{ wff,                580 },
			{ bf_or,              720 },
			{ bf_and,             730 },
			{ bf_xor,             740 },
			{ bf_neg,             750 },
			{ bf,                 790 },
			{ bf_matcher,         795 },
			{ bf_body,            795 },
			{ rec_relation,       800 },
			{ ref_args,           800 },
			{ bf_rule,            800 },
			{ wff_rule,           800 },
			{ binding,            800 },
		};
		
		if (no_wrap_for.find(nt) != no_wrap_for.end())
			return false;
		auto p_it = prio.find(pt);
		auto n_it   = prio.find(nt);
		if (p_it == prio.end() || n_it == prio.end()) {
#ifdef DEBUG
			std::cerr << "No priority for " << (p_it == prio.end()
					? tau_parser::instance().name(pt)
					: tau_parser::instance().name(nt)) << "\n";
#endif
			return false;
		}
		// tau_parser& p = tau_parser::instance();
		// std::cerr << "\n"
		// 	<< p.name(parent_type) << " vs " << p.name(node_type)
		// 	//<< "(" << parent_type << ")
		// 	<< " " << parent_it->second
		// 	<< (parent_it->second > node_it->second
		// 		? " > " : " <= ")
		// 	<< node_it->second
		// 	// << " (" << node_type << ")"
		// 	<< "\n";
		return p_it->second > n_it->second;
	};

	auto indent = [&depth, &os]() {
		if (!pretty_printer_indenting) return;
		for (size_t i = 0; i < depth; ++i) os << "\t";
	};
	auto break_line = [&os, &indent]() {
		if (!pretty_printer_indenting) return;
		os << "\n", indent();
	};
	// auto break_if_needed = [&break_line](size_t nt) -> bool {
	// 	static const std::vector<size_t> breaks = {
	// 		wff_and, wff_or, wff_xor, wff_imply, wff_equiv,
	// 		wff_all, wff_ex
	// 	};
	// 	if (!pretty_printer_indenting) return false;
	// 	if (find(breaks.begin(), breaks.end(), nt)
	// 		!= breaks.end()) return break_line(), true;
	// 	return false;
	// };
	auto inc_indent = [&](size_t nt) {
		static const std::vector<size_t> indents = {
			wff_sometimes, wff_always, wff_conditional,
			wff_all, wff_ex, wff_imply, wff_equiv
		};
		bool ret = pretty_printer_indenting &&
			find(indents.begin(), indents.end(), nt)
							!= indents.end();
		if (ret) depth++;
		return ret;
	};

	static const std::map<size_t, std::string> hl_colors = {
		{ bf,            idni::TC.LIGHT_GREEN() },
		{ variable,      idni::TC.WHITE() },
		{ capture,       idni::TC.BLUE() },
		{ wff_all,       idni::TC.MAGENTA() },
		{ wff_ex,        idni::TC.LIGHT_MAGENTA() },

		{ rec_relation,  idni::TC.YELLOW() },
		{ constraint,    idni::TC.LIGHT_MAGENTA() },
		{ io_var,        idni::TC.WHITE() },
		{ constant,      idni::TC.LIGHT_CYAN() }

		// { rule,          idni::TC.BG_YELLOW() },
		// { builder,       idni::TC.BG_LIGHT_YELLOW() }
	};
	auto syntax_highlight = [&](size_t nt) {
		if (!pretty_printer_highlighting) return false;
		if (auto it = hl_colors.find(nt); it == hl_colors.end())
			return false;
		else return hl_path.push_back(nt), os << it->second, true;
	};

	auto on_enter = [&](tref ref, tref parent) {
		const auto& t = get(ref);
#ifdef DEBUG_TRAVERSAL
		std::cerr << "[" << t.get_type_name() << "]";
#endif
		// t.print_tree(os << "entering: ") << "\n";
		size_t nt = t.get_type();

		auto track_chpos = [&]() { chpos[ref] = 0; };

		if (inc_indent(nt)) indented.insert(ref);
		if (syntax_highlight(nt)) highlighted.insert(ref);

		switch (nt) {
			case bf_f:              os << '0'; break;
			case bf_t:              os << '1'; break;
			case wff_f:             os << 'F'; break;
			case wff_t:             os << 'T'; break;
			case wff_neg:           os << "!"; break;
			case bf_neg:            os << "'"; break;
			case first_sym:         os << "first"; break;
			case last_sym:          os << "last"; break;
			case fp_fallback:       os << " fallback "; break;
			// wraps
			case builder_head:
			case ref_args:          os << "("; break;
			case constraint:
			case offsets:           os << "["; break;
			case bf_splitter:       os << "S("; break;
			case bf_constant:       os << "{ "; break;
			case bf:
			case wff:
				if (parent && is_to_wrap(t.first_tree().get_type(),
					get(parent).get_type()))
				{
					wraps.insert(ref), os << "(";
					if (static_cast<node::type>(nt) == wff) depth++, break_line();
				}
				break;

			case wff_all:           os << "all "; break;
			case wff_ex:            os << "ex "; break;

			case wff_sometimes:     os << "sometimes "; break;
			case wff_always:        os << "always "; break;

			case bf_builder_body:   os << " =: "; break;
			case wff_builder_body:  os << " =:: "; break;

			case rel_memory:        os << "%-"; break;
			case abs_memory:        os << "%"; break;
			case dnf_cmd:           os << "dnf "; break;
			case cnf_cmd:           os << "cnf "; break;
			case anf_cmd:           os << "anf "; break;
			case nnf_cmd:           os << "nnf "; break;
			case pnf_cmd:           os << "pnf "; break;
			case mnf_cmd:           os << "mnf "; break;
			case snf_cmd:           os << "snf "; break;
			case onf_cmd:           os << "onf "; break;
			case def_print_cmd:
			case def_rr_cmd:
			case def_list_cmd:      os << "def"; break;
			case history_list_cmd:  os << "history"; break;
			case history_print_cmd:
			case history_store_cmd: os << "history "; break;
			case get_cmd:           os << "get"; break;
			case set_cmd:           os << "set "; break;
			case toggle_cmd:        os << "toggle "; break;
			case quit_cmd:          os << "quit"; break;
			case version_cmd:       os << "version"; break;
			case clear_cmd:         os << "clear"; break;
			case help_cmd:          os << "help"; break;
			case file_cmd:          os << "file"; break;
			case valid_cmd:         os << "valid "; break;
			case sat_cmd:           os << "sat "; break;
			case unsat_cmd:         os << "unsat "; break;
			case solve_cmd:         os << "solve "; break;
			case run_cmd:           os << "run "; break;
			case normalize_cmd:     os << "normalize "; break;
			case inst_cmd:          track_chpos(); os << "instantiate "; break;
			case subst_cmd:         track_chpos(); os << "substitute "; break;
			case wff_conditional:   track_chpos(); break;
			default:
				if (is_string_nt(nt)) os << string_from_id(t.data());
				else if (is_digital_nt(nt)) os << t.data();
				else if (t.is_integer()) os << t.get_integer();
				else if (t.is_ba_constant()) os << "{C"
					<< t.get_ba_constant_id() << "}";
		}
		return true;
	};
	auto on_between = [&](tref /*left*/, tref parent) {
		const auto& t = get(parent);
#ifdef DEBUG_TRAVERSAL
		std::cerr << "[|" << t.get_type_name() << "] \n";
#endif
		auto inc_chpos = [&chpos, &parent]() { return chpos[parent]++; };
		auto chpos_end = [&chpos, &parent]() { chpos.erase(parent); };

		size_t nt = t.get_type();
		switch (nt) {
			case bf_and:            break;
			case bf_or:             os << "|"; break;
			case bf_xor:            os << "+"; break;
			case bf_eq:             os << " = "; break;
			case bf_neq:            os << " != "; break;
			case bf_less_equal:     os << " <= "; break;
			case bf_nleq:           os << " !<= "; break;
			case bf_greater:        os << " > "; break;
			case bf_ngreater:       os << " !> "; break;
			case bf_greater_equal:  os << " >= "; break;
			case bf_ngeq:           os << " !>= "; break;
			case bf_less:           os << " < "; break;
			case bf_nless:          os << " !< "; break;

			case ctn_neq:           os << " != "; break;
			case ctn_eq:            os << " = "; break;
			case ctn_greater_equal: os << " >= "; break;
			case ctn_greater:       os << " > "; break;
			case ctn_less_equal:    os << " <= "; break;
			case ctn_less:          os << " < "; break;

			case wff_and:           os << " && "; break;
			case wff_or:            os << " || "; break;
			case wff_xor:           os << " ^ "; break;
			case wff_imply:         os << " -> "; break;
			case wff_equiv:         os << " <-> "; break;

			case bf_interval:       os << " <= "; break;

			case rec_relation:      os << " := "; break;
			case wff_rule:          os << " ::= "; break;
			case bf_rule:           os << " := "; break;
			case in:
			case out:               os << "["; break;
			case shift:             os << "-"; break;
			case bf_constant:       os << " : "; break;

			case cli:               os << ". "; break;
			case wff_conditional:
				if (inc_chpos()) chpos_end(), os << " : ";
				else os << " ? ";
				break;
			case inst_cmd:
			case subst_cmd:
				switch (inc_chpos()) {
					case 1: os << " ["; break;
					case 2: os << " / "; break;
					case 3: chpos_end(); break;
				}
		};
		return true;
	};
	auto on_leave = [&](tref ref) {
		const auto& t = get(ref);
#ifdef DEBUG_TRAVERSAL
		std::cerr << "\n[/" << t.get_type_name() << "]";
#endif
		// t.print_tree( << "leaving: ") << "\n";
		size_t nt = t.get_type();
		switch (nt) {
			case main: 
			case builder:
			case rec_relation:
			case wff_rule:
			case bf_rule:      os << "."; break;
			case in:
			case out:
			case constraint:
			case offsets:
			case inst_cmd:
			case subst_cmd:    os << "]"; break;
			case bf_splitter:
			case builder_head:
			case ref_args:     os << ")"; break;
			case bf_constant:  os << " }"; break;
			case bf:
			case wff:
				if (wraps.find(ref) != wraps.end()) {
					wraps.erase(ref), os << ")";
					if (static_cast<node::type>(nt) == wff) depth--, break_line();
				}
				break;
		}
		if (pretty_printer_highlighting
			&& highlighted.find(ref) != highlighted.end())
		{
			hl_path.pop_back(), os << TC.CLEAR();
			if (hl_path.size()) os << hl_colors.at(hl_path.back());
			highlighted.erase(ref);
		}
		if (pretty_printer_indenting
			&& indented.find(ref) != indented.end())
				depth--, indented.erase(ref);
	};
	auto visit_all = [](tref) { return true; };
	pre_order<node>(this->get()).visit(on_enter, visit_all,
						on_leave, on_between);
	return os;
}

template <typename node>
std::ostream& operator<<(std::ostream& os, const tree<node>& t) {
	return t.print(os);
}

template <typename node>
std::ostream& operator<<(std::ostream& os,
	const typename tree<node>::traverser& t)
{
	if (!t.has_value()) return os << "(no value)";
	if (t.values().size() == 1) return os << tree<node>::get(t.value());
	for (const auto& c : t.values()) os << tree<node>::get(c) << "\n";
	return os << "\n\n";
}

} // idni::tau_lang namespace
