// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// Tau traverser

// -----------------------------------------------------------------------------
// extractor

template <NodeType node>
template <typename result_type>
tree<node>::extractor<result_type>::extractor(const function& e)
	: e(e) {}

template <NodeType node>
template <typename result_type>
result_type tree<node>::extractor<result_type>::operator()(
	const traverser& t) const
{
	return e(t);
}

// -----------------------------------------------------------------------------
// extractors

template <NodeType node>
const typename tree<node>::template extractor<tref>
	tree<node>::traverser::ref =
		typename tree<node>::template extractor<tref>(
			[](const traverser& t) -> tref {
				return t.value();
			});

template <NodeType node>
const typename tree<node>::template extractor<trefs>
	tree<node>::traverser::refs =
		typename tree<node>::template extractor<trefs>(
			[](const traverser& t) -> trefs {
				return t.values();
			});

template <NodeType node>
const typename tree<node>::template extractor<htree::sp>
	tree<node>::traverser::handle =
		typename tree<node>::template extractor<htree::sp>(
			[](const traverser& t) {
				if (!t) return htree::sp();
				return geth(t.value());
			});

template <NodeType node>
const typename tree<node>::template extractor<const tree<node>&>
	tree<node>::traverser::Tree =
		typename tree<node>::template extractor<const tree<node>&>(
			[](const traverser& t) -> const tree<node>& {
				return t.value_tree();
			});

template <NodeType node>
const typename tree<node>::template extractor<typename tree<node>::traverser>
	tree<node>::traverser::dump =
		typename tree<node>::template extractor<traverser>(
			[](const traverser& t) {
				if (!t) return t;
				t.value_tree().dump(std::cout) << "\n";
				return t;
			});

template <NodeType node>
const typename tree<node>::template extractor<typename tree<node>::traverser>
	tree<node>::traverser::print_tree =
		typename tree<node>::template extractor<traverser>(
			[](const traverser& t) {
				if (!t) return t;
				t.value_tree().print_tree(std::cout) << "\n";
				return t;
			});

template <NodeType node>
const typename tree<node>::template extractor<typename node::type>
	tree<node>::traverser::nt =
		typename tree<node>::template extractor<typename node::type>(
			[](const traverser& t) {
				if (!t) return
					static_cast<typename node::type>(0);
				return t.value_tree().get_type();
			});

template <NodeType node>
const typename tree<node>::template extractor<std::string>
	tree<node>::traverser::string =
		typename tree<node>::template extractor<std::string>(
			[](const traverser& t) {
				if (!t) return std::string();
				return t.value_tree().get_string();
			});

template <NodeType node>
const typename tree<node>::template extractor<int_t>
	tree<node>::traverser::integer =
		typename tree<node>::template extractor<int_t>(
			[](const traverser& t) {
				if (!t) return 0;
				return t.value_tree().get_integer();
			});

template <NodeType node>
const typename tree<node>::template extractor<size_t>
	tree<node>::traverser::num =
		typename tree<node>::template extractor<size_t>(
			[](const traverser& t) -> size_t {
				if (!t) return 0;
				return t.value_tree().get_num();
			});

template <NodeType node>
const typename tree<node>::template extractor<size_t>
	tree<node>::traverser::data =
		typename tree<node>::template extractor<size_t>(
			[](const traverser& t) -> size_t {
				if (!t) return 0;
				return t.value_tree().data();
			});

template <NodeType node>
const typename tree<node>::template extractor<size_t>
	tree<node>::traverser::ba_constant_id =
		typename tree<node>::template extractor<size_t>(
			[](const traverser& t) -> size_t {
				if (!t) return 0;
				return t.value_tree().get_ba_constant_id();
			});

template <NodeType node>
const typename tree<node>::template extractor<size_t>
	tree<node>::traverser::ba_type =
		typename tree<node>::template extractor<size_t>(
			[](const traverser& t) -> size_t {
				if (!t) return 0;
				return t.value_tree().get_ba_type();
			});

template <NodeType node>
const typename tree<node>::template extractor<typename tree<node>::bas_variant>
	tree<node>::traverser::ba_constant =
		typename tree<node>::template extractor<bas_variant>(
			[](const traverser& t) -> bas_variant {
				return ba_constants_t::get(
					t.value_tree().get_ba_constant_id());
			});

template <NodeType node>
const typename tree<node>::template extractor<typename tree<node>::traverser>
	tree<node>::traverser::only_child =
		typename tree<node>::template extractor<traverser>(
			[](const traverser& t) -> traverser {
				if (!t) return {};
				tref r = t.value_tree().only_child();
				if (!r) return {};
				return traverser(r);
			});

template <NodeType node>
const typename tree<node>::template extractor<typename tree<node>::traverser>
	tree<node>::traverser::first =
		typename tree<node>::template extractor<traverser>(
			[](const traverser& t) {
				if (!t) return traverser();
				tref r = t.value_tree().first();
				if (!r) return traverser();
				return traverser(r);
			});

template <NodeType node>
const typename tree<node>::template extractor<typename tree<node>::traverser>
	tree<node>::traverser::second =
		typename tree<node>::template extractor<traverser>(
			[](const traverser& t) {
				if (!t) return traverser();
				tref r = t.value_tree().second();
				if (!r) return traverser();
				return traverser(r);
			});

template <NodeType node>
const typename tree<node>::template extractor<typename tree<node>::traverser>
	tree<node>::traverser::third =
		typename tree<node>::template extractor<traverser>(
			[](const traverser& t) {
				if (!t) return traverser();
				tref r = t.value_tree().third();
				if (!r) return traverser();
				return traverser(r);
			});

template <NodeType node>
const typename tree<node>::template extractor<typename tree<node>::traverser>
	tree<node>::traverser::children =
		typename tree<node>::template extractor<traverser>(
			[](const traverser& t) {
				if (!t) return traverser();
				return traverser(t.value_tree()
						.get_children());
			});

template <NodeType node>
const typename tree<node>::template extractor<tref_range<node>>
	tree<node>::traverser::children_range =
		typename tree<node>::template extractor<tref_range<node>>(
			[](const traverser& t) {
				if (!t) return traverser();
				return traverser(t.value_tree()
						.children());
			});

template <NodeType node>
const typename tree<node>::template extractor<tree_range<tree<node>>>
	tree<node>::traverser::children_trees_range =
		typename tree<node>::template extractor<tree_range<tree<node>>>(
			[](const traverser& t) {
				if (!t) return traverser();
				return traverser(t.value_tree()
						.children_trees());
			});

template <NodeType node>
const typename tree<node>::template extractor<typename tree<node>::traverser>
	tree<node>::traverser::f(const auto& fn)
{
	return extractor<traverser>([fn](const traverser& t) {
		return traverser(fn(t.value()));
	});
}

//------------------------------------------------------------------------------
// traverser

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
tref tree<node>::traverser::value() const {
	DBG(assert(has_value());)
	return values_.front();
}

template <NodeType node>
const tree<node>& tree<node>::traverser::value_tree() const {
	DBG(assert(has_value());)
	return get(values_.front());
}

template <NodeType node>
const tree<node>& tree<node>::traverser::operator[](size_t n) const {
	DBG(assert(has_value());)
	return value_tree()[n];
}

template <NodeType node>
trefs tree<node>::traverser::values() const { return values_; }

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
bool tree<node>::traverser::is(size_t nt) const {
	DBG(assert(has_value());)
	return value_tree().is(nt);
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
