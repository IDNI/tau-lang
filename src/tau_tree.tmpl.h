// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"
#include "hooks.h"

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
const tree<node>& tree<node>::get(const htree::sp& h) {
	return (const tree&) base_t::get(h);
}
template <NodeType node>
htree::sp tree<node>::geth(tref h) {
	DBG(assert(h != nullptr);)
	return base_t::geth(h);
}

//------------------------------------------------------------------------------
// creation with tref childs

template <NodeType node>
tref tree<node>::get_raw(const node& v, const tref* ch, size_t len, tref r) {
	// BOOST_LOG_TRIVIAL(debug) << "get_raw: " << v << " " << len << " r: " << r << "\n";
	return base_t::get(v, ch, len, r);
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
	static get_hook<node> hook;
	return hook(v, ch, len, r);
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
	return get(v, ch1, ch2);
}

template <NodeType node>
tref tree<node>::get(const node& v, const node* ch, size_t len, tref r)
{
	tref pr = nullptr;
	for (size_t i = len; i > 0; ) --i,
		pr = get(ch[i], (tref) nullptr, pr);
	return get(v, &pr, 1, r);
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
tref tree<node>::get(const node::type& nt, const std::string& s) {
	return get(node(nt, string_id(s)));
}

//------------------------------------------------------------------------------
// terminals

template <NodeType node>
tref tree<node>::get_num(size_t n) {
	return get(node(tree<node>::num, n));
}

template <NodeType node>
tref tree<node>::get_integer(int_t n) {
	return get(node(integer, static_cast<size_t>(n)));
}

template <NodeType node>
tref tree<node>::get_ba_constant(size_t c) {
	return get_ba_constant(c, 0);
}

template <NodeType node>
tref tree<node>::get_ba_constant(size_t c, size_t tid) {
	return get(node::ba_constant(c, tid));
}

template <NodeType node>
tref tree<node>::get_ba_constant(const std::pair<size_t, size_t>& typed_const) {
	return get(node::ba_constant(typed_const.first, typed_const.second));
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

//------------------------------------------------------------------------------
// nt category helpers

template <NodeType node>
bool tree<node>::is_string_nt(size_t nt) {
	static const std::set<size_t> string_nts{
		sym, type, source, named_binding, capture, variable,
		in_var_name, out_var_name, uninter_const_name, file_name,
		ctnvar, option_name, option_value
	};
	return string_nts.contains(nt);
}

template <NodeType node>
bool tree<node>::is_digital_nt(size_t nt) {
	static const std::set<size_t> digital_nts{
		num, memory_id
	};
	return digital_nts.contains(nt);
}

template <NodeType node>
bool tree<node>::is_term_nt(size_t nt, size_t parent_nt) {
	switch (nt) {
		case bf:
		case bf_constant:
		case bf_splitter:
		case bf_ref:
		case bf_or:
		case bf_xor:
		case bf_and:
		case bf_neg:
		case bf_t:
		case bf_f:
		case variable:
			return true;
		case capture: if (parent_nt == bf) return true; break;
		case ref: if (parent_nt == bf_ref) return true; break;
		default: ;
	}
	return false;
}

//------------------------------------------------------------------------------
// fast access helpers

template <NodeType node>
size_t tree<node>::data() const { return this->value.data; }

template <NodeType node>
size_t tree<node>::child_data() const { return first_tree().value.data; }

template <NodeType node>
bool tree<node>::is(size_t nt) const {
	return this->value.nt == nt;
}

template <NodeType node>
bool tree<node>::is_string() const { return is_string_nt(this->value.nt); }

template <NodeType node>
bool tree<node>::is_integer() const { return is(integer); }

template <NodeType node>
bool tree<node>::is_num() const { return is(num); }

template <NodeType node>
bool tree<node>::is_ba_constant() const { return is(bf_constant); }

template <NodeType node>
bool tree<node>::child_is(size_t nt) const {
	if (only_child() == nullptr) return false;
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
	return string_from_id(this->value.data);
}

template <NodeType node>
int_t tree<node>::get_integer() const {
	DBG(assert(is_integer());)
	return static_cast<int_t>(this->value.data);
}

template <NodeType node>
size_t tree<node>::get_num() const {
	DBG(assert(is_num());)
	return this->value.data;
}

template <NodeType node>
tref tree<node>::get(const node::bas_variant& c, size_t type) {
	return node::ba_constants_binder_t::instance().bind(c, type);
}

template <NodeType node>
size_t tree<node>::get_ba_constant_id() const {
	DBG(assert(is_ba_constant());)
	return this->value.data;
}

template <NodeType node>
tree<node>::bas_variant tree<node>::get_ba_constant() const {
	DBG(assert(is_ba_constant());)
	return ba_constants_t::get(data());
}

template <NodeType node>
size_t tree<node>::get_ba_type() const {
	return this->value.ba;
}

} // idni::tau_lang namespace
