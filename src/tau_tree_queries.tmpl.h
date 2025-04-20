// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

template <BAsPack... BAs>
bool is_non_terminal(tref n) {
	return tree<node<BAs...>>::get(n).is_nt();
}

template <BAsPack... BAs>
bool is_terminal(tref n) {
	return tree<node<BAs...>>::get(n).is_t();
}

template <BAsPack... BAs>
bool is_non_terminal(const size_t nt, tref n) {
	return tree<node<BAs...>>::get(n).is(nt);
}

template <size_t nt, typename... BAs>
bool is_non_terminal(tref n) { return is_non_terminal<BAs...>(nt, n); }

// factory method for is_non_terminal predicate
template <BAsPack... BAs>
inline std::function<bool(tref)> is_non_terminal(size_t nt) {
	return [nt](tref n) { return is_non_terminal<BAs...>(nt, n); };
}

} // idni::tau_lang namespace
