// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The concrete BA packs' specializations are DECLARED in these headers and
// defined once each in the matching .cpp (see those files for why).  Included
// outside the namespace below because they open their own; an explicit
// specialization must be declared in the namespace its template belongs to.
#include "base_ba_dispatcher_bv_sbf.h"
#include "base_ba_dispatcher_full_pack.h"
#include "base_ba_dispatcher_bv_bool.h"

namespace idni::tau_lang {

// Parse a plain Bool constant ("0"/"false"/"F" or "1"/"true"/"T").
template <typename... BAs>
requires BAsPack<BAs...>
inline std::optional<typename node<BAs...>::constant_with_type> parse_bool(
		const std::string& src) {
	if (src == "0" || src == "false" || src == "F")
		return typename node<BAs...>::constant_with_type{
			std::variant<BAs...>{ Bool(false) }, bool_type<node<BAs...>>() };
	if (src == "1" || src == "true" || src == "T")
		return typename node<BAs...>::constant_with_type{
			std::variant<BAs...>{ Bool(true) }, bool_type<node<BAs...>>() };
	return {};
}

/*template<>
struct base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba> {};

template<>
struct base_ba_dispatcher<bv, sbf_ba> {};*/

} // namespace idni::tau_lang
