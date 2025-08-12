// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"
#include "interpreter_types.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// Tau tree implementation declarations
// -----------------------------------------------------------------------------

struct rr_sig;
template <NodeType node> struct ref_types;
template <NodeType node> struct ba_types;
template <NodeType node> struct ba_constants;
template <NodeType node> struct get_hook;

// -----------------------------------------------------------------------------
// various extractors (tau_tree_extractors.tmpl.h)
template <NodeType node>
size_t get_type_sid(tref n);

template <NodeType node>
rr_sig get_rr_sig(tref n);

template <NodeType node>
bool get_io_def(tref def, io_defs<node>& in_or_out_defs);

template <NodeType node>
bool get_io_defs(spec_context<node>& ctx, tref code);

template <NodeType node>
rewriter::rules get_rec_relations(spec_context<node>& ctx, tref r);
template <NodeType node>
rewriter::rules get_rec_relations(tref r);

template <NodeType node>
std::optional<rr<node>> get_nso_rr(spec_context<node>& ctx, tref ref,
				   bool wo_inference = false);
template <NodeType node>
std::optional<rr<node>> get_nso_rr(tref ref, bool wo_inference = false);
template <NodeType node>
std::optional<rr<node>> get_nso_rr(const rewriter::rules& rules, tref main_fm);
template <NodeType node>
std::optional<rr<node>> get_nso_rr(const rewriter::rules& rules,
				   const htref& main_fm);

template <NodeType node>
void get_leaves(tref n, typename node::type branch, trefs& leaves);
template <NodeType node>
trefs get_leaves(tref n, typename node::type branch);

template <NodeType node>
trefs get_dnf_wff_clauses(tref n);
template <NodeType node>
trefs get_cnf_wff_clauses(tref n);
template <NodeType node>
trefs get_dnf_bf_clauses(tref n);
template <NodeType node>
trefs get_cnf_bf_clauses(tref n);

template <NodeType node>
size_t get_ba_type(tref n);

template <NodeType node>
tref get_var_name_node(tref var);
template <NodeType node>
const std::string& get_var_name(tref var);
template <NodeType node>
size_t get_var_name_sid(tref var);

template <NodeType node>
bool is_io_initial(tref io_var);
template <NodeType node>
bool is_io_shift(tref io_var);
template <NodeType node>
int_t get_io_time_point(tref io_var);
template <NodeType node>
int_t get_io_shift(tref io_var);
template <NodeType node>
int_t get_io_var_shift(tref io_var);
template <NodeType node>
int_t get_max_shift(const trefs& io_vars, bool ignore_temps = false);
template <NodeType node>
int_t get_max_initial(const trefs& io_vars);

template <NodeType node>
subtree_set<node> get_free_vars_from_nso(tref n);

template <NodeType node>
bool has_temp_var(tref n);

template <NodeType node>
bool has_open_tau_fm_in_constant(tref fm);

} // namespace idni::tau_lang

#include "dict.h"      // string pool accessed with dict() functions
#include "rr.h"        // recurrence relations structure
#include "ba_types.h"  // ba types dictionary

#include "tau_tree_builders.h"

#include "ref_types_inference.h"
#include "ba_types_inference.h"

#include "tau_tree_node.tmpl.h"
#include "tau_tree_traverser.tmpl.h"
#include "tau_tree_printers.tmpl.h"
#include "tau_tree_queries.tmpl.h"
#include "tau_tree_extractors.tmpl.h"  // TODO rename this file to proper name?
#include "tau_tree_from_parser.tmpl.h"

#include "hooks.h"

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
	if (!use_hooks) return get_raw(v, ch, len, r);
	get_hook<node> hook;
	// set hook first if not hooked
	if (!base_t::is_hooked()) base_t::set_hook(
		[&hook](const node& v, const tref* ch, size_t len, tref r) {
			return hook(v, ch, len, r);
		});
	// get with hooks
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
	return get(v, ch1, ch2);
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
tref tree<node>::get_num(size_t n) {
	return get(node(tree<node>::num, n));
}

template <NodeType node>
tref tree<node>::get_integer(int_t n) {
	return get(node(integer, static_cast<size_t>(n)));
}

template <NodeType node>
tref tree<node>::get_ba_constant(
	const constant& constant, const std::string& type_name)
{
	LOG_TRACE << " -- get ba_constant(constant constant, string type_name): `"
		<< LOG_BA(constant) << "`, " << LOG_BA(type_name);
	return ba_constants<node>::get(constant,
				       get_ba_type_id<node>(type_name));
}

template <NodeType node>
tref tree<node>::get_ba_constant(const constant& constant, size_t ba_type_id)
{
	LOG_TRACE << " -- get ba_constant(constant constant, size_t ba_type_id): `"
		<< LOG_BA(constant) << "`, " << LOG_BA_TYPE(ba_type_id);
	return ba_constants<node>::get(constant, ba_type_id);
}

template <NodeType node>
tref tree<node>::get_ba_constant(
	const std::string& constant_source,
	const std::string  type_name)
{
	LOG_TRACE << " -- get ba_constant(string constant_source, string type_name): `"
		<< constant_source << "`, " << LOG_BA(type_name);
	return get_ba_constant_from_source(dict(constant_source),
		get_ba_type_id<node>(type_name));
}

template <NodeType node>
tref tree<node>::get_ba_constant_from_source(
	size_t constant_source_sid,
	size_t ba_type_id)
{
	LOG_TRACE << " -- get ba_constant_from_source(size_t sid, size_t tid): `"
				<< dict(constant_source_sid) << "`, "
				<< LOG_BA_TYPE(ba_type_id) << " " << ba_type_id;

	if (ba_type_id == 0)
		LOG_TRACE << " -- untyped: " << dict(constant_source_sid);
	else LOG_TRACE << " -- typed: " << ba_types<node>::name(ba_type_id);

	tref r = ba_type_id == 0
		? get( // untyped contains source sid
			node::ba_constant(constant_source_sid, ba_type_id))
		: get_ba_constant(
				node::nso_factory::instance().parse(
					dict(constant_source_sid),
					ba_types<node>::name(ba_type_id)));
	if (r == nullptr) LOG_ERROR << "Parsing constant `"
		<< dict(constant_source_sid) << "` failed for type `"
		<< ba_types<node>::name(ba_type_id) << "`.";
	else LOG_TRACE << " -- result: " << LOG_FM(r);
	return r;
}

template <NodeType node>
tref tree<node>::get_ba_constant(size_t constant_id, size_t ba_type_id) {
	LOG_TRACE << " -- get_ba_constant(size_t constant_id, size_t ba_type_id): `"
		<< LOG_BA(ba_constants<node>::get(constant_id)) << "`, "
		<< LOG_BA_TYPE(ba_type_id);
	return get_ba_constant(ba_constants<node>::get(constant_id), ba_type_id);
}

template <NodeType node>
tref tree<node>::get_ba_constant(
	const std::pair<constant, std::string>& typed_const)
{
	LOG_TRACE << " -- get_ba_constant(pair<constant, string>): `"
		<< LOG_BA(typed_const.first) << "`, " << LOG_BA(typed_const.second);
	return get_ba_constant(typed_const.first,
			       get_ba_type_id<node>(typed_const.second));
}

template <NodeType node>
tref tree<node>::get_ba_constant(
	const std::optional<std::pair<constant, std::string>>& typed_const)
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
		sym, type, source, capture, var_name, uconst_name, file_name,
		ctnvar, option_name, option_value
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
bool tree<node>::is_term_nt(size_t nt, size_t parent_nt) {
	switch (nt) {
		case bf:
		case bf_constant:
		case bf_splitter:
		case bf_fall:
		case bf_fex:
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
bool tree<node>::is_num() const { return is(num) || is(history_id); }

template <NodeType node>
bool tree<node>::is_ba_constant() const { return is(bf_constant); }

template <NodeType node>
bool tree<node>::is_term() const { return this->value.term || is(io_var); }

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
	return *this == tree<node>::get_0();
}

template <NodeType node>
bool tree<node>::equals_1() const {
	return *this == tree<node>::get_1();
}

template <NodeType node>
bool tree<node>::equals_F() const {
	return *this == tree<node>::get_F();
}

template <NodeType node>
bool tree<node>::equals_T() const {
	return *this == tree<node>::get_T();
}

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
	return dict(this->value.data);
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
tree<node>::constant tree<node>::get_ba_constant() const {
	DBG(assert(is_ba_constant());)
	return ba_constants<node>::get(data());
}

template <NodeType node>
size_t tree<node>::get_ba_type() const {
	if (this->value.nt == bf) return child_tree(0).get_ba_type();
	return this->value.ba;
}

template <NodeType node>
const std::string& tree<node>::get_ba_type_name() const {
	return ba_types<node>::name(this->get_ba_type());
}

} // namespace idni::tau_lang

template<typename... BAs> requires idni::tau_lang::BAsPack<BAs...>
size_t std::hash<idni::tau_lang::node<BAs...>>::operator()(
	const idni::tau_lang::node<BAs...>& n) const noexcept {
	return n.hash;
}
