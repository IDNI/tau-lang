// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Explicit specializations of the base BA dispatcher and constant parser for
// the <bv, Bool> pack.
//
// These used to be defined `inline` in base_ba_dispatcher.tmpl.h, which every
// test translation unit includes.  Their bodies call parse_bv/parse_sbf/
// parse_tau over the pack, so merely *seeing* them instantiated the entire
// <bv, sbf_ba> pipeline in every TU -- measured with clang -ftime-trace at
// 40.6%% of all template-instantiation time in a TU that does not use the pack
// at all, and 1158 KB / 5972 emitted weak symbols per object.
//
// Only four targets actually use this pack (tests/test_sbf_ba_helpers.h and
// tests/integration/solver/test_integration-solver-sbf.cpp), so the
// definitions now live in one translation unit compiled into libTAU and every
// other TU sees declarations only.
//
// plugins_compile_time.md R3; plugins_infra.md R6 wants this glue deleted
// outright once the pack becomes a configure-time choice.

#ifndef __IDNI__TAU__BASE_BA_DISPATCHER_BV_BOOL_H__
#define __IDNI__TAU__BASE_BA_DISPATCHER_BV_BOOL_H__

namespace idni::tau_lang {

template <>
std::optional<ba_constants<node<bv, Bool>>::constant_with_type> ba_constants<node<bv, Bool>>::get( const std::string& constant_source, tref type_tree, [[maybe_unused]] const std::string options);

template <>
bool base_ba_dispatcher<bv, Bool>::is_syntactic_one(const std::variant<bv, Bool>& elem);

template <>
bool base_ba_dispatcher<bv, Bool>::is_syntactic_zero(const std::variant<bv, Bool>& elem);

template <>
bool base_ba_dispatcher<bv, Bool>::is_one(const std::variant<bv, Bool>& elem);

template <>
bool base_ba_dispatcher<bv, Bool>::is_zero(const std::variant<bv, Bool>& elem);

template <>
bool base_ba_dispatcher<bv, Bool>::is_closed(const std::variant<bv, Bool>&);

template <>
std::vector<std::string> base_ba_dispatcher<bv, Bool>::types();

template <>
tref base_ba_dispatcher<bv, Bool>::default_type();

template <>
std::string base_ba_dispatcher<bv, Bool>::one(const tref type_tree);

template <>
std::string base_ba_dispatcher<bv, Bool>::zero(const tref type_tree);

template <>
tref base_ba_dispatcher<bv, Bool>::splitter_one(tref);

template <>
std::variant<bv, Bool> base_ba_dispatcher<bv, Bool>::splitter(const std::variant<bv, Bool>& elem, splitter_type);

template <>
tref base_ba_dispatcher<bv, Bool>::unpack_tau_ba(const std::variant<bv, Bool>&);

template <>
std::variant<bv, Bool> base_ba_dispatcher<bv, Bool>::pack_tau_ba(tref);

template <>
std::variant<bv, Bool> base_ba_dispatcher<bv, Bool>::normalize(const std::variant<bv, Bool>& v);

template <>
tref base_ba_dispatcher<bv, Bool>::simplify_symbol(tref symbol);

template <>
tref base_ba_dispatcher<bv, Bool>::simplify_term(tref term);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BASE_BA_DISPATCHER_BV_BOOL_H__
