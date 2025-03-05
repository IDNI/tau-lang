// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <memory>

#include "../queries.h"

namespace idni::tau_lang {

using parse_forest = idni::parser<char, char>::pforest;
using parse_result = idni::parser<char, char>::result;

template <typename...BAs>
std::optional<tau<BAs...>> bitvector_ba_factory<BAs...>::parse(
	const std::string& /*src*/)
{
	// TODO implement
	return {};
}

template <typename...BAs>
tau<BAs...> bitvector_ba_factory<BAs...>::binding(const tau<BAs...>& /*sn*/) {
	// TODO implement
	return nullptr;
}

template <typename...BAs>
std::variant<BAs...> bitvector_ba_factory<BAs...>::splitter_one() const {
	throw std::logic_error("Unsupported");
}

template <typename...BAs>
std::string bitvector_ba_factory<BAs...>::one() const { return "1:bit[]"; }

template <typename...BAs>
std::string bitvector_ba_factory<BAs...>::zero() const { return "0:bit[]"; }

} // namespace idni::tau_lang
