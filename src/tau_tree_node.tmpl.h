// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

namespace idni::tau_lang {

// Tau tree node templates implementation
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
	static_assert(sizeof...(BAs) > 0,
		"Empty template parameter pack not allowed");
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

} // idni::tau_lang namespace
