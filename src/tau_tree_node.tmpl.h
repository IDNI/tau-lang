// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "ba_constants.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "node"

namespace idni::tau_lang {

// Tau tree node templates implementation
// -----------------------------------------------------------------------------

template <typename... BAs>
requires BAsPack<BAs...>
node<BAs...> node<BAs...>::retype(size_t new_nt) const {
	return node(new_nt, data, term, ba_type, ext);
}

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
// operators and capture. Those are typed later in the pipeline, and the
// DBG-only "all term nodes are typed" invariant in
// tree<node>::get(const parser::tree&, get_options&) (tau_tree_from_parser.tmpl.h)
// relies on this narrower set to avoid false positives on not-yet-typed nodes.
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

template <typename... BAs>
requires BAsPack<BAs...>
constexpr node<BAs...> node<BAs...>::nnull() { return node(); }

template <typename... BAs>
requires BAsPack<BAs...>
constexpr node<BAs...> node<BAs...>::extension(T raw_value) {
	// ba_type is not encoded in raw_value (see extension() below), so it
	// cannot be recovered here and is reset to 0.
	return node(
		(raw_value >> node::nt_shift) & node::nt_mask,
		raw_value & node::data_mask,
		(raw_value >> node::term_shift) & 1u,
		0,
		(raw_value >> node::ext_shift) & 1u
	);
}

#define NODE_CAST(x) static_cast<node<BAs...>::T>(x)
template <typename... BAs>
requires BAsPack<BAs...>
constexpr node<BAs...>::T node<BAs...>::extension() const noexcept {
	T result = 0;
	result |= (NODE_CAST(nt) & ((1u << node::nt_bits) - 1u)) << node::nt_shift;
	result |= (NODE_CAST(term) & 1u) << node::term_shift ;
	result |= (NODE_CAST(ext) & 1u) << node::ext_shift;
	result |= NODE_CAST(data) & node::data_mask;
	return result;
}

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
constexpr size_t node<BAs...>::hashit() const {
	std::size_t seed = 0;
	hash_combine(seed, static_cast<size_t>(nt));
	// term bit is derived from nt via is_term_nt() and intentionally excluded
	// hash_combine(seed, static_cast<bool>(term));
	// Hash ba_type directly (integer).  The BA-type IDs are assigned
	// deterministically in ba_types::type_tree_to_idx, so identical node
	// shapes yield identical hashes without having to stringify the type
	// (which can invoke tree::print on an uninitialised type-tree and
	// crash on the small variants used in unit tests).
	hash_combine(seed, ba_type);
	hash_combine(seed, static_cast<bool>(ext));
	// Get ba constant from pool
	if (nt == type::ba_constant && data != 0 && ba_type != 0)
		hash_combine(seed, tau_lang::ba_constants<node>::get(data));
	// Get string from pool, untyped ba_constant also has string as data
	else if (tree<node>::is_string_nt(nt) || nt == type::ba_constant)
		hash_combine(seed, dict(data));
	else hash_combine(seed, static_cast<size_t>(data));
	return seed;
}

} // namespace idni::tau_lang
