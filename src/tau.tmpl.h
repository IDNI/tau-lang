#include "tau.h"

namespace idni::tau_lang {

static inline bool is_string_nt(size_t nt) {
	using p = node::type;
	static const std::set<size_t> string_nts{
		p::sym, p::type, p::source, p::named_binding, p::capture,
		p::variable, p::in_var_name, p::out_var_name,
		p::uninter_const_name, p::file_name, p::ctnvar, p::option_name,
		p::option_value
	};
	return string_nts.contains(nt);
}

static inline bool is_digital_nt(size_t nt) {
	using p = node::type;
	static const std::set<size_t> digital_nts{
		p::num, p::memory_id
	};
	return digital_nts.contains(nt);
}

std::ostream& operator<<(std::ostream& os, const node& n) {
	if (n.nt == node::type::integer) os << n.as_int() << " ";
	else if (n.nt == node::type::bf_constant) os << " {C" << n.data << "} ";
	else if (is_digital_nt(n.nt)) os << n.data << " ";
	else if (is_string_nt(n.nt)) os << "\"" << S[n.data] << "\" ";
	// else if (n.ext) os << "{EXT}";
	os << tau_parser::instance().name(n.nt);
#ifdef DEBUG
	if (bool print_nt_ids = true; print_nt_ids) os << "(" << n.nt << ")";
	if (n.data) os << " #" << n.data;
#endif
	return os;
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
tref tree<node>::get(const node::type& nt, const std::string& s) {
	size_t sid = 0;
	if (auto it = SM.find(s); it != SM.end()) sid = it->second;
	else sid = (SM.emplace(s, S.size()), S.push_back(s), S.size() - 1);
	return get(node(nt, sid));
}

//------------------------------------------------------------------------------
// terminals

template <typename node>
tref tree<node>::get_num(size_t n) {
	return get(node(tree<node>::num, n));
}

template <typename node>
tref tree<node>::get_integer(int_t n) {
	return get(node(node::type::integer, static_cast<size_t>(n)));
}

template <typename node>
tref tree<node>::get_ba_constant_id(size_t c) {
	return get(node(node::type::bf_constant, c));
}

//------------------------------------------------------------------------------
// transform from tau_parser::tree

// Converts parser tree nodes to tau tree nodes
// - terminals are colapsed into their parents
//     ie. shift > num > "10" becomes shift > num("10")
//     or  variable > type > "sbf" becomes variable > type("sbf")
// important terminals are stored outside of the tree and value is actually its
// id (strings, constants)
// for constants, binder requires type and a tref, store it and get the id
template <typename node>
template <typename binder>
tref tree<node>::get(const tau_parser::tree& pt, binder& bind) {
	using ptree = tau_parser::tree;
	std::unordered_map<tref, tref> m;
	auto transformer = [&m, &bind](tref t) {
		// std::cout << "transforming:" << tau_parser::tree::get(t).value << std::endl;
		if (m.find(t) != m.end()) return true;
		const auto& tr = ptree::get(t);
		const auto& l = tr.value.first;
		if (!l.nt()) return true;
		tref x;
		if (l.n() == node::type::integer) {
			bool neg = false;
			tref d = nullptr;
			for (tref c : tr.children()) {
				if (d != nullptr) neg = true;
				d = c;
			}
			int_t i = static_cast<int_t>(std::stoul(
				ptree::get(d).get_terminals()));
			x = get_integer(neg ? -i : i);
		} else if (l.n() == node::type::bf_constant) {
			// call binder with a type and a tref, store it and get the id
			size_t cid = bind(t);
			if (bind.error) return false;
			x = get_ba_constant_id(cid);
		} else if (is_digital_nt(l.n())) {
			size_t v = std::stoul(tr.get_terminals());
			x = get(node(l.n(), v));
		} else if (is_string_nt(l.n()))
			x = get(static_cast<node::type>(l.n()),
							tr.get_terminals());
		else {
			trefs ch;
			for (tref c : tr.children()) ch.push_back(m.at(c));
			auto nt = l.n();
			switch (nt) { // transform intermediate nodes
				case node::type::bf_and_nosep_1st_oprnd:
				case node::type::bf_and_nosep_2nd_oprnd:
				case node::type::bf_neg_oprnd:
					nt = node::type::bf; break;
				case node::type::bf_and_nosep:
					nt = node::type::bf_and; break;
			}
			x = get(static_cast<node::type>(nt), ch);
		}
		m.emplace(t, x);
		// std::cout << "\tto: " << tree::get(m.at(t)).value << std::endl;
		return true;
	};
	// tau_parser::tree::get(pt.get()).print(std::cout << "tau tree from parser tree: ") << std::endl;
	idni::post_order<tau_parser::pnode>(pt.get()).search(transformer);
	return m.at(pt.get());
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

//------------------------------------------------------------------------------
// print

template <typename node>
std::ostream& tree<node>::print(std::ostream& os, size_t s) const {
	for (size_t i = 0; i < s; i++) os << "\t";
	os << this->value << "\n";
	for (const auto& ch : children()) get(ch).print(os, s + 1);
	return os;
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
bool tree<node>::is_integer() const { return is(node::type::integer); }

template <typename node>
bool tree<node>::is_num() const { return is(node::type::num); }

template <typename node>
bool tree<node>::is_ba_constant() const { return is(node::type::bf_constant); }

template <typename node>
node::type tree<node>::get_type() const {
	return static_cast<node::type>(this->value.nt);
}

template <typename node>
const std::string& tree<node>::get_string() const {
	DBG(assert(is_string());)
	return S[this->value.data];
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
	for (const auto& v : values())
		for (const tref& c : get(v).children()) {
			if (get(c).is(nt)) r.push_back(c); 
		}
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

}