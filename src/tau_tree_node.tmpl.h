// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_constants.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "node"

namespace idni::tau_lang {

// Tau tree node templates implementation
// -----------------------------------------------------------------------------

template <typename... BAs>
requires BAsPack<BAs...>
node<BAs...> node<BAs...>::retype(size_t new_nt) const {
	return node(new_nt, data, term, ba, ext);
}

template <typename... BAs>
requires BAsPack<BAs...>
node<BAs...> node<BAs...>::ba_retype(size_t new_ba) const {
	DBG(assert(new_ba <= node<BAs...>::ba_max));
	return node(nt, data, term, new_ba, ext);
}

template <typename... BAs>
requires BAsPack<BAs...>
node<BAs...> node<BAs...>::replace_data(size_t new_data) const {
	return node(nt, new_data, term, ba, ext);
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
	auto n = node(type::bf_constant, constant_id,
		    true /* is_term */, ba_type_id);
	// LOG_TRACE << " -- node::ba_constant result:" << n;
	return n;
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

inline bool is_term_nt(size_t nt) {
	switch (nt) {
		case tau_parser::bf:
		case tau_parser::bf_constant:
		case tau_parser::bf_splitter:
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
	: nt(nt), term(is_term || is_term_nt(nt)), ba(ba_type), ext(ext), data(data),
		hash(hashit())
{
	static_assert(sizeof...(BAs) > 0,
	"Empty template parameter pack not allowed");
	// DBG(LOG_TRACE << "ba: " << ba_type);
	// DBG(LOG_TRACE << "ba_max: " << node<BAs...>::ba_max);
	DBG(assert(ba_type <= node<BAs...>::ba_max));
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
requires BAsPack<BAs...>
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
requires BAsPack<BAs...>
std::weak_ordering node<BAs...>::operator<=>(const node& that) const {
	// if (hash != that.hash) return hash    <=> that.hash;
	if (nt   != that.nt)   return C(nt)   <=> C(that.nt);
	if (term != that.term) return C(term) <=> C(that.term);
	if (ba   != that.ba)   return C(ba)   <=> C(that.ba);
	if (ext  != that.ext)  return C(ext)  <=> C(that.ext);
	// if (tree<node>::is_string_nt(nt))
	// 	return dict(data) <=> dict(that.data);
	return C(data) <=> C(that.data);
}
#undef C
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
	// if (nt == type::bf_f && that.nt == type::bf_f) // 0 - ignore ba type if any untyped
	// 	return (ba > 0 && that.ba > 0) ? ba == that.ba : true;
	// if (nt == type::bf_t && that.nt == type::bf_t) // 1 - ignore ba type if any untyped
	// 	return (ba > 0 && that.ba > 0) ? ba == that.ba : true;
	return nt == that.nt && term == that.term && ba == that.ba
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
	hash_combine(seed, static_cast<bool>(nt));
	hash_combine(seed, static_cast<bool>(term));
	// In order to have a deterministic hash, we hash the type name
	hash_combine(seed, get_ba_type_name<node>(ba));
	hash_combine(seed, static_cast<bool>(ext));
	// Get ba constant from pool
	if (nt == type::bf_constant && data != 0 && ba != 0)
		hash_combine(seed, ba_constants<node>::get(data));
	// Get string from pool, untyped bf_constant also has string as data
	else if (tree<node>::is_string_nt(nt) || nt == type::bf_constant)
		hash_combine(seed, dict(data));
	else hash_combine(seed, data);
	return seed;
}

} // namespace idni::tau_lang
