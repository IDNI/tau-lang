// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BA_TYPE_TRAITS_H__
#define __IDNI__TAU__BA_TYPE_TRAITS_H__

// Type traits expressing the BA type hierarchy.
//
// omcat (ω-categorical)
//   ├── aba  (atomless BA): qint, nlang_ba, sbf_ba, tau_ba<...>
//   └── non_aba_omcat     : qlt, (future non-aba omcats)
// bv  (atomic BA, not omcat)

#include <type_traits>
#include "boolean_algebras/qlt.h"
#include "boolean_algebras/qint.h"
#include "boolean_algebras/nlang_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/tau_ba.h"

namespace idni::tau_lang {

// is_aba: atomless Boolean algebras
template <typename T> struct is_aba : std::false_type {};
template <> struct is_aba<qint>     : std::true_type {};
template <> struct is_aba<nlang_ba> : std::true_type {};
template <> struct is_aba<sbf_ba>   : std::true_type {};
template <typename... BAs> struct is_aba<tau_ba<BAs...>> : std::true_type {};
template <typename T> inline constexpr bool is_aba_v = is_aba<T>::value;

// is_non_aba_omcat: ω-categorical types that are NOT Boolean algebras
template <typename T> struct is_non_aba_omcat : std::false_type {};
template <> struct is_non_aba_omcat<qlt> : std::true_type {};
template <typename T> inline constexpr bool is_non_aba_omcat_v = is_non_aba_omcat<T>::value;

// is_omcat: any ω-categorical type (aba or non_aba_omcat)
template <typename T> struct is_omcat
	: std::disjunction<is_aba<T>, is_non_aba_omcat<T>> {};
template <typename T> inline constexpr bool is_omcat_v = is_omcat<T>::value;

// Static assertions to verify the traits at compile time
static_assert(is_aba_v<qint>,     "qint must be aba");
static_assert(is_aba_v<nlang_ba>, "nlang_ba must be aba");
static_assert(is_aba_v<sbf_ba>,   "sbf_ba must be aba");
static_assert(is_non_aba_omcat_v<qlt>, "qlt must be non_aba_omcat");
static_assert(is_omcat_v<qlt>,    "qlt must be omcat");
static_assert(is_omcat_v<qint>,   "qint must be omcat");

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BA_TYPE_TRAITS_H__
