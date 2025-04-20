// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// Tau tree templates implementation

//------------------------------------------------------------------------------
// handles

template <NodeType node>
tref tree<node>::get() const { return base_t::get(); }
template <NodeType node>
const tree<node>& tree<node>::get(const tref id) {
	return (const tree&) base_t::get(id);
}
template <NodeType node>
const tree<node>& tree<node>::get(const htree::sp& h) {
	return (const tree&) base_t::get(h);
}
template <NodeType node>
htree::sp tree<node>::geth(tref h) {return base_t::geth(h);}

//------------------------------------------------------------------------------
// creation with tref childs

template <NodeType node>
tref tree<node>::get(const node& v) { return base_t::get(v); }

template <NodeType node>
tref tree<node>::get(const node& v, tref child) {
	return base_t::get(v, child);
}

template <NodeType node>
tref tree<node>::get(const node& v, tref ch1, tref ch2) {
	return base_t::get(v, ch1, ch2);
}

template <NodeType node>
tref tree<node>::get(const node& v, const tref* ch, size_t len) {
	return base_t::get(v, ch, len);
}

template <NodeType node>
tref tree<node>::get(const node& v, const trefs& children) {
	return base_t::get(v, children);
}

template <NodeType node>
tref tree<node>::get(const node& v, const std::initializer_list<tref>& ch) {
	return base_t::get(v, ch);
}


//------------------------------------------------------------------------------
// creation with node childs

template <NodeType node>
tref tree<node>::get(const node& v, const node& child) {
	return base_t::get(v, child);
}

template <NodeType node>
tref tree<node>::get(const node& v, const node& ch1, const node& ch2)
{
	return get(v, ch1, ch2);
}

template <NodeType node>
tref tree<node>::get(const node& v, const node* ch, size_t len)
{
	return base_t::get(v, ch, len);
}

template <NodeType node>
tref tree<node>::get(const node& v, const std::vector<node>& ch)
{
	return base_t::get(v, ch);
}

template <NodeType node>
tref tree<node>::get(const node& v, const std::initializer_list<node>& ch)
{
	return base_t::get(v, ch);
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
tref tree<node>::get(const node::type& nt, const tref* ch, size_t len) {
	return get(node(nt), ch, len);
}

template <NodeType node>
tref tree<node>::get(const node::type& nt, const trefs& ch) {
	return get(node(nt), ch);
}

template <NodeType node>
tref tree<node>::get(const node::type& nt,
	const std::initializer_list<tref>& ch)
{
	return get(node(nt), ch);
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

//------------------------------------------------------------------------------

template <NodeType node>
template <typename binder>
tref tree<node>::get(binder& bind, tau_parser::result& result) {
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
		return nullptr; // Syntax error
	}
	auto pt = parse_tree::get(result.get_shaped_tree2());
	return tree<node>::template get<binder>(bind, pt);
}

template <NodeType node>
tref tree<node>::get(tau_parser::result& result) {
	return tree<node>::get<ba_constants_binder_t>(
		ba_constants_binder_t::instance(), result);
}

template <NodeType node>
template <typename binder>
tref tree<node>::get(binder& bind, const std::string& source,
	parse_options options)
{
	auto result = tau_parser::instance()
		.parse(source.c_str(), source.size(), options);
	return tree<node>::template get<binder>(bind, result);
}

template <NodeType node>
tref tree<node>::get(const std::string& source, parse_options options) {
	return tree<node>::get<ba_constants_binder_t>(
		ba_constants_binder_t::instance(), source, options);
}

template <NodeType node>
template <typename binder>
tref tree<node>::get(binder& bind, std::istream& is, parse_options options) {
	auto result = tau_parser::instance().parse(is, options);
	return tree<node>::template get<binder>(bind, result);
}

template <NodeType node>
tref tree<node>::get(std::istream& is, parse_options options) {
	return tree<node>::get<ba_constants_binder_t>(
		ba_constants_binder_t::instance(), is, options);
}

template <NodeType node>
template <typename binder>
tref tree<node>::get_from_file(binder& bind, const std::string& filename,
	parse_options options)
{
	auto result = tau_parser::instance().parse(filename, options);
	return tree<node>::template get<binder>(bind, result);
}

template <NodeType node>
tref tree<node>::get_from_file(const std::string& filename, parse_options options){
	return tree<node>::get<ba_constants_binder_t>(
		ba_constants_binder_t::instance(), filename, options);
}

template <NodeType node>
rewriter::rules tree<node>::get_rec_relations(tref rrs) {
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

template <NodeType node>
void tree<node>::get_leaves(tref n, node::type branch, trefs& leaves) {
	auto add_leave = [&branch, &leaves](tref n) {
		const auto& t = tree<node>::get(n);
		if (t.is(branch)) return true;
		if (t.child_is(branch)) return true;
		return leaves.push_back(n), false;
	};
	pre_order<node>(n).visit(add_leave);
}

template <NodeType node>
trefs tree<node>::get_leaves(tref n, node::type branch) {
	trefs leaves;
	get_leaves(n, branch, leaves);
	return leaves;
}

template <NodeType node>
trefs tree<node>::get_dnf_wff_clauses(tref n) {
	return get_leaves(n, tau_parser::wff_or);
}

template <NodeType node>
trefs tree<node>::get_dnf_bf_clauses(tref n) {
	return get_leaves(n, tau_parser::bf_or);
}

template <NodeType node>
trefs tree<node>::get_cnf_wff_clauses(tref n) {
	return get_leaves(n, tau_parser::wff_and);
}

template <NodeType node>
trefs tree<node>::get_cnf_bf_clauses(tref n) {
	return get_leaves(n, tau_parser::bf_and);
}

template <NodeType node>
rewriter::rules tree<node>::get_rules(tref r) {
	auto rs = tt(r) | rules || rule || tt::first;
	rewriter::rules x;
	for (auto r : rs.traversers()) {
		// tree::get(r.value()).print(std::cout << "rule: ");
		x.emplace_back(tree::geth(r | tt::first | tt::ref),
				tree::geth(r | tt::second | tt::ref));
	}
	return x;
}

template <NodeType node>
template <typename binder>
rewriter::library tree<node>::get_library(binder& bind, const std::string& str) {
	return get_rules(tree::get<binder>(bind, str, { .start=library }));
}

template <NodeType node>
rewriter::library tree<node>::get_library(const std::string& str) {
	return tree<node>::get_library<ba_constants_binder_t>(
		ba_constants_binder_t::instance(), str);
}

template <NodeType node>
template <typename binder>
rewriter::builder tree<node>::get_builder(binder& bind, const std::string& source){
	return tree<node>::get_builder(get<binder>(bind, source, {
					.start = tau_parser::builder }));
}

template <NodeType node>
rewriter::builder tree<node>::get_builder(const std::string& source) {
	return tree<node>::get_builder<ba_constants_binder_t>(
		ba_constants_binder_t::instance(), source);
}

template <NodeType node>
rewriter::builder tree<node>::get_builder(tref ref) {
	DBG(assert(ref != nullptr);)
	tt b(ref);
	auto body = b | builder_body | tt::only_child;
	DBG(assert((body | tt::nt) == bf_builder_body
		|| (body | tt::nt) == wff_builder_body);)
	return { geth(b | builder_head | tt::ref),
		geth(body | tt::first | tt::ref) };
}

template <NodeType node>
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
size_t tree<node>::get_ba_constant_id() const {
	DBG(assert(is_ba_constant());)
	return this->value.data;
}

template <NodeType node>
tree<node>::bas_variant tree<node>::get_ba_constant() const {
	DBG(assert(is_ba_constant());)
	return ba_constants_t::get(data());
}

//------------------------------------------------------------------------------

template <NodeType node>
tree<node>::traverser::traverser() : has_value_(false) {}
template <NodeType node>
tree<node>::traverser::traverser(tref r) : has_value_(r != nullptr),
			values_(has_value_ ? trefs{ r } : trefs{}) {}
template <NodeType node>
tree<node>::traverser::traverser(const htree::sp& h) : has_value_(h != nullptr),
			values_(has_value_ ? trefs{ h->get() } : trefs{}) {}
template <NodeType node>
tree<node>::traverser::traverser(const trefs& refs) { set_values(refs); }

template <NodeType node>
bool tree<node>::traverser::has_value() const { return has_value_; }
template <NodeType node>
tree<node>::traverser::operator bool() const { return has_value(); }

template <NodeType node>
tref tree<node>::traverser::value() const { return values_.front(); }

template <NodeType node>
const tree<node>& tree<node>::traverser::value_tree()
	const { return get(values_.front()); }

template <NodeType node>
const tree<node>& tree<node>::traverser::operator[](
	size_t n) const { return value_tree()[n]; }

template <NodeType node>
const trefs& tree<node>::traverser::values() const { return values_; }

template <NodeType node>
std::vector<typename tree<node>::traverser>
	tree<node>::traverser::traversers() const
{
	std::vector<traverser> tv;
	for (const auto& v : values_) tv.emplace_back(v);
	return tv;
}

template <NodeType node>
std::vector<typename tree<node>::traverser>
	tree<node>::traverser::operator()() const
{
	return traversers();
}

template <NodeType node>
bool tree<node>::traverser::empty() const { return values_.empty(); }

template <NodeType node>
size_t tree<node>::traverser::size() const { return values_.size(); }

template <NodeType node>
typename tree<node>::traverser
	tree<node>::traverser::operator|(size_t nt) const
{
	if (!has_value()) return traverser();
	for (tref c : get(value()).children()) {
		if (get(c).is(nt)) return { c }; 
	}
	return {};
}

template <NodeType node>
typename tree<node>::traverser
	tree<node>::traverser::operator||(size_t nt) const
{
	trefs r;
	for (tref v : values())	for (tref c : get(v).children())
			if (get(c).is(nt)) r.push_back(c);
	return traverser(r);
}

template <NodeType node>
template <typename result_type>
result_type tree<node>::traverser::operator|(
	const extractor<result_type>& e) const
{
	return e(*this);
}

template <NodeType node>
template <typename result_type>
result_type tree<node>::traverser::operator||(
	const extractor<result_type>& e) const
{
	return e(*this);
}

template <NodeType node>
void tree<node>::traverser::set_values(const trefs& refs) {
	for (tref t : refs) if (t != nullptr) values_.push_back(t);
	has_value_ = values_.size();
}

} // idni::tau_lang namespace
