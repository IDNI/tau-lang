// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "ba_constants.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "node"

namespace idni::tau_lang {

// Tau tree node templates implementation
// -----------------------------------------------------------------------------

template <typename... BAs>
requires BAsPack<BAs...>
node<BAs...> node<BAs...>::ba_retype(size_t new_ba) const {
	return node(nt, data, term, new_ba, ext);
}

template <typename... BAs>
requires BAsPack<BAs...>
node<BAs...> node<BAs...>::replace_data(size_t new_data) const {
	return node(nt, new_data, term, ba_type, ext);
}

template <typename... BAs>
requires BAsPack<BAs...>
constexpr node<BAs...> node<BAs...>::ba_constant(
	size_t constant_id, size_t ba_type_id)
{
	// LOG_TRACE << " -- node::ba_constant: constant_id: "
	// 	<< constant_id << " : " << LOG_BA_TYPE_DUMP(ba_type_id);
	// LOG_TRACE << " -- node::bitsizes: " << node::nt_bits << "/1/"
	// 	  << node::ba_bits << "/1/" << node::data_bits;
	return node(type::ba_constant, constant_id, true /* is_term */, ba_type_id);
	// LOG_TRACE << " -- node::ba_constant result:" << n;
}

template <typename... BAs>
requires BAsPack<BAs...>
constexpr node<BAs...> node<BAs...>::ba_typed(type nt, size_t ba_tid) {
	return node(nt, 0, true /* is_term */, ba_tid);
}

template <typename... BAs>
requires BAsPack<BAs...>
constexpr node<BAs...> node<BAs...>::input_variable(size_t ba_tid)
{
	return node(type::io_var, 1 /* input */, true, ba_tid);
}

template <typename... BAs>
requires BAsPack<BAs...>
constexpr node<BAs...> node<BAs...>::output_variable(size_t ba_tid)
{
	return node(type::io_var, 2 /* output */, true, ba_tid);
}

// Deliberately narrower than tree<node>::is_term_nt (see tau_tree.tmpl.h):
// this variant only sets the node's `term` bit at construction time for the
// core bf/io_var nonterminals, excluding the extended bf arithmetic/functional
// operators and capture. NOTE (TT1-18): the DBG "all term nodes are typed"
// invariant in tree<node>::get(const parser::tree&, get_options&)
// (tau_tree_from_parser.tmpl.h) does NOT use this function -- unqualified
// lookup there resolves to the wide member tree::is_term_nt, and that is
// load-bearing: this narrower set flags atoms (e.g. qlt ordering operands)
// that are legitimately still untyped at that stage.
inline bool is_term_nt(size_t nt) {
	switch (nt) {
		case tau_parser::bf:
		case tau_parser::ba_constant:
		case tau_parser::bf_ref:
		case tau_parser::bf_or:
		case tau_parser::bf_xor:
		case tau_parser::bf_and:
		case tau_parser::bf_neg:
		case tau_parser::bf_t:
		case tau_parser::bf_f:
		case tau_parser::variable:
		case tau_parser::io_var:
			return true;
		default:
			return false;
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
constexpr node<BAs...>::node(size_t nt, size_t data, size_t is_term,
		size_t ba_type, size_t ext) noexcept
	: nt(nt), term(is_term || is_term_nt(nt)), ext(ext), data(data), ba_type(ba_type),
		hash(hashit())
{
	static_assert(sizeof...(BAs) > 0,
	"Empty template parameter pack not allowed");
	// DBG(LOG_TRACE << "ba: " << ba_type);
	// DBG(LOG_TRACE << "ba_max: " << node<BAs...>::ba_max);
		// LOG_TRACE << "node created:" << LOG_NT(nt)
	// 	<< " data: " << data
	// 	<< " is_term: " << is_term
	// 	<< " ba_type: " << LOG_BA_TYPE(ba_type)
	// 	<< " node: `" << *this << "`";
}

template <typename... BAs>
requires BAsPack<BAs...>
const std::string& node<BAs...>::name() const {
	return name(nt);
}

template <typename... BAs>
requires BAsPack<BAs...>
const std::string& node<BAs...>::name(size_t nt) {
	return tau_parser::instance().name(nt);
}

template <typename... BAs>
requires BAsPack<BAs...>
int_t node<BAs...>::as_int() const { return static_cast<int_t>(data); }

// (TT1-11: nnull() and the extension() pack/unpack pair deleted -- zero
// callers, and the packing lost nt's MSB; see the note in tau_tree.h.)

#define NODE_CAST(x) static_cast<node<BAs...>::T>(x)

template <typename... BAs>
requires BAsPack<BAs...>
std::weak_ordering node<BAs...>::operator<=>(const node& that) const {
	// The hash comparison is needed in order to ensure determinism due to
	// due to different possible storage positions of strings and BA constants
	if (hash != that.hash) return hash    <=> that.hash;
	if (nt   != that.nt)   return NODE_CAST(nt)   <=> NODE_CAST(that.nt);
	// term bit is derived from nt via is_term_nt() and intentionally excluded
	//if (term != that.term) return NODE_CAST(term) <=> NODE_CAST(that.term);
	if (ba_type   != that.ba_type)   return NODE_CAST(ba_type)   <=> NODE_CAST(that.ba_type);
	if (ext  != that.ext)  return NODE_CAST(ext)  <=> NODE_CAST(that.ext);
	return NODE_CAST(data) <=> NODE_CAST(that.data);
}
#undef NODE_CAST
template <typename... BAs>
requires BAsPack<BAs...>
constexpr bool node<BAs...>::operator<(const node& that) const {
	return (*this <=> that) < 0;
}
template <typename... BAs>
requires BAsPack<BAs...>
constexpr bool node<BAs...>::operator<=(const node& that) const {
	return (*this <=> that) <= 0;
}
template <typename... BAs>
requires BAsPack<BAs...>
constexpr bool node<BAs...>::operator>(const node& that) const {
	return (*this <=> that) > 0;
}
template <typename... BAs>
requires BAsPack<BAs...>
constexpr bool node<BAs...>::operator>=(const node& that) const {
	return (*this <=> that) >= 0;
}
template <typename... BAs>
requires BAsPack<BAs...>
constexpr auto node<BAs...>::operator==(const node& that) const {
	// term bit is derived from nt via is_term_nt() and intentionally excluded
	return nt == that.nt /*&& term == that.term*/ && ba_type == that.ba_type
			&& ext == that.ext && data == that.data;
}
template <typename... BAs>
requires BAsPack<BAs...>
constexpr auto node<BAs...>::operator!=(const node& that) const {
	return !(*this == that);
}
template <typename... BAs>
requires BAsPack<BAs...>
size_t node<BAs...>::hashit() const {
	std::size_t seed = 0;
	hash_combine(seed, static_cast<size_t>(nt));
	// term bit is derived from nt via is_term_nt() and intentionally excluded
	// hash_combine(seed, static_cast<bool>(term));
	// Hash ba_type directly (integer). Hashing the type NAME instead --
	// via ba_types<node>::name_hash(), which is defined and available --
	// makes the hash stable across runs where a parameterized type's id
	// depends on first-discovery order. That is the better property, but
	// it yields a DIFFERENT canonical order than the one this tree's
	// order-sensitive expectations are written against: hashit feeds node
	// interning, hence tref values, hence subtree_less and pivot
	// selection, hence printed shape. Switching it re-baselines the
	// accepted-ordering lists in test_tau_bdd, the cqe antiprenexing
	// cases, simplify_using_equality and the interpreter value checks --
	// deliberate work to do on its own, not folded into a merge, so that
	// an ordering shift stays distinguishable from a real regression.
	hash_combine(seed, ba_type);
	hash_combine(seed, static_cast<bool>(ext));
	// Get ba constant from pool (ba_constant.data is always a ba_constants
	// pool index, regardless of ba_type -- see node::ba_constant()).
	if (nt == type::ba_constant && data != 0)
		hash_combine(seed, tau_lang::ba_constants<node>::get(data));
	// Get string from pool
	else if (tree<node>::is_string_nt(nt))
		hash_combine(seed, dict(data));
	else hash_combine(seed, static_cast<size_t>(data));
	return seed;
}

} // namespace idni::tau_lang
