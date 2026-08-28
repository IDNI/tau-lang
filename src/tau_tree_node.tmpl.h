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
node<BAs...> node<BAs...>::replace_data(T new_data) const {
	return node(nt, new_data, term, ba_type, ext);
}

template <typename... BAs>
requires BAsPack<BAs...>
constexpr node<BAs...> node<BAs...>::ba_constant(
	T constant_id, size_t ba_type_id)
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
constexpr node<BAs...>::node(size_t nt, T data, size_t is_term,
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
	// data is a pool index for string nts and ba_constant, not a value --
	// comparing it raw orders by interning position, exactly the
	// storage-position dependence this function's hash comparison above
	// is meant to avoid. Dereference to the pooled value where possible.
	// Strings are ordered, so this closes the gap for them; ba_constant's
	// pooled type is a variant over the configured BA pack, and packs are
	// only required to be equality_comparable (ba_descriptor_complete),
	// not ordered -- bv, a default-pack BA, has no operator<=>, so an
	// ordered variant comparison is ill-formed there. Raw data is the
	// only option left for ba_constant. This is a determinism fix, not a
	// hash-primitive choice, so it applies under every policy.
	if (tree<node>::is_string_nt(nt)) return dict(data) <=> dict(that.data);
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
uint64_t node<BAs...>::hashit() const {
	std::uint64_t seed = 0;
	hash_combine(seed, static_cast<size_t>(nt));
	// term bit is derived from nt via is_term_nt() and intentionally excluded
	// hash_combine(seed, static_cast<bool>(term));
	// Hash the type name: a parameterized type's id follows first-discovery
	// order, so hashing the id makes the hash vary between runs.
	hash_combine(seed, tau_lang::ba_types<node>::name_hash(ba_type));
	hash_combine(seed, static_cast<bool>(ext));
	// Get ba constant from pool (ba_constant.data is always a ba_constants
	// pool index, regardless of ba_type -- see node::ba_constant()).
	// Hashed by content, not identity: bintree::operator< compares hashes
	// first, so a constant's default std::hash could make DNF clause order
	// -- and the witness the interpreter picks for a free output -- depend
	// on process history (GitHub #89). A BA whose std::hash isn't already
	// content-derived says so with hash_constant on its descriptor (see
	// ba_has_hash_constant_v); every other BA falls back to std::hash<BA>.
	if (nt == type::ba_constant && data != 0) {
		const auto c = tau_lang::ba_constants<node>::get(data);
		hash_combine(seed, c.index(), std::visit([](const auto& v) {
			using BA = std::decay_t<decltype(v)>;
			if constexpr (ba_has_hash_constant_v<node, BA>)
				return ba_descriptor<BA, node>::hash_constant(v);
			else return std::hash<BA>{}(v);
		}, c));
	}
	// Get string from pool
	else if (tree<node>::is_string_nt(nt))
		hash_combine(seed, dict(data));
	else hash_combine(seed, static_cast<size_t>(data));
	return seed;
}

} // namespace idni::tau_lang
